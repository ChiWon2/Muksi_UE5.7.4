#include "Controllers/PlayerMode/PlayerMode_World.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/Travel/Public/Camera/WanderingCameraManager.h"
#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"
#include "Muksi/Dialogue/DialogueSubsystem.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "UnrealClient.h"

void UPlayerMode_World::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	if (!PC || !PC->IsLocalController())
		return;

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	LastRightClickMoveRequestTime = -1.0f;
	ClearTrackedDestination();
	SetTrackedPathFollowing(nullptr);

	PreviousViewTarget = PC->GetViewTarget();
	CreateWanderingCamera();
	UpdateWanderingCamera();

	if (AWorldUIManager* UIManager = FindWorldUIManager())
	{
		WorldMapMarkerUI = UIManager->GetWorldMapMarkerUIComponent();
		if (WorldMapMarkerUI.IsValid())
			WorldMapMarkerUI->StartPresentation(PC, WanderingCameraManager);
	}
}

void UPlayerMode_World::ExitMode()
{
	ClearTrackedDestination();
	SetTrackedPathFollowing(nullptr);

	if (WorldMapMarkerUI.IsValid())
		WorldMapMarkerUI->StopPresentation();

	WorldMapMarkerUI.Reset();

	if (IsValid(WanderingCameraManager))
	{
		WanderingCameraManager->EndDrag();

		if (PC && PC->GetViewTarget() == WanderingCameraManager)
		{
			AActor* RestoreTarget = PreviousViewTarget.IsValid() ? PreviousViewTarget.Get() : PC->GetPawn();
			PC->SetViewTarget(RestoreTarget);
		}

		WanderingCameraManager->Destroy();
	}

	WanderingCameraManager = nullptr;
	PreviousViewTarget.Reset();
	LastRightClickMoveRequestTime = -1.0f;

	Super::ExitMode();
}

void UPlayerMode_World::TickPlayerMode()
{
	Super::TickPlayerMode();
	UpdateWanderingCamera();
	UpdateDestinationState();
}

void UPlayerMode_World::HandleLeftClick(const FInputActionValue& Value)
{
	if (CanProcessWorldInput() && IsValid(WanderingCameraManager))
		WanderingCameraManager->BeginDrag();
}

void UPlayerMode_World::HandleLeftClickReleased(const FInputActionValue& Value)
{
	if (IsValid(WanderingCameraManager))
		WanderingCameraManager->EndDrag();
}

void UPlayerMode_World::HandleRightClick(const FInputActionValue& Value)
{
	LastRightClickMoveRequestTime = -1.0f;
	HandleRightClickTriggered(Value);
}

void UPlayerMode_World::HandleRightClickTriggered(const FInputActionValue& Value)
{
	if (!CanProcessWorldInput())
		return;

	AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(PC->GetPawn());
	if (!WorldCharacter)
		return;

	UWorld* World = PC->GetWorld();
	if (!World)
		return;

	const float CurrentTime = World->GetTimeSeconds();
	if (LastRightClickMoveRequestTime >= 0.0f && CurrentTime - LastRightClickMoveRequestTime < FMath::Max(RightClickMoveUpdateInterval, 0.01f))
		return;

	LastRightClickMoveRequestTime = CurrentTime;

	FVector HitLocation = FVector::ZeroVector;
	FVector MarkerPosition = FVector::ZeroVector;
	if (TryGetCursorDestination(HitLocation, MarkerPosition))
		RequestWorldMove(HitLocation, MarkerPosition);
}

void UPlayerMode_World::RequestWorldMove(const FVector& Destination, const FVector& MarkerPosition)
{
	if (!PC || !PC->GetPawn())
		return;

	UPathFollowingComponent* PreviousComponent = PC->FindComponentByClass<UPathFollowingComponent>();
	const FAIRequestID PreviousRequest = PreviousComponent ? PreviousComponent->GetCurrentRequestId() : FAIRequestID::InvalidRequest;
	const FAIRequestID PreviouslyTrackedRequest = ActiveMoveRequest;

	// SimpleMove aborts the previous request synchronously before starting its replacement.
	ActiveMoveRequest = FAIRequestID::InvalidRequest;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, Destination);

	UPathFollowingComponent* PathFollowing = PC->FindComponentByClass<UPathFollowingComponent>();
	SetTrackedPathFollowing(PathFollowing);
	if (!PathFollowing || PathFollowing->GetStatus() == EPathFollowingStatus::Idle)
	{
		ClearTrackedDestination();
		return;
	}

	const FAIRequestID CurrentRequest = PathFollowing->GetCurrentRequestId();
	if (!CurrentRequest.IsValid())
	{
		ClearTrackedDestination();
		return;
	}

	// An early failure can leave an existing request running; do not label it with the rejected destination.
	if (PathFollowing == PreviousComponent && CurrentRequest.GetID() == PreviousRequest.GetID())
	{
		if (CurrentRequest.GetID() == PreviouslyTrackedRequest.GetID())
			ActiveMoveRequest = PreviouslyTrackedRequest;
		else
			ClearTrackedDestination();

		return;
	}

	ActiveMoveRequest = CurrentRequest;
	TrackedMovePawn = PC->GetPawn();
	if (WorldMapMarkerUI.IsValid())
		WorldMapMarkerUI->ShowDestination(MarkerPosition);
}

void UPlayerMode_World::SetTrackedPathFollowing(UPathFollowingComponent* PathFollowing)
{
	if (TrackedPathFollowing.Get() == PathFollowing)
		return;

	if (TrackedPathFollowing.IsValid())
		TrackedPathFollowing->OnRequestFinished.RemoveAll(this);

	TrackedPathFollowing = PathFollowing;
	if (PathFollowing)
		PathFollowing->OnRequestFinished.AddUObject(this, &UPlayerMode_World::HandleWorldMoveFinished);
}

void UPlayerMode_World::HandleWorldMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!ActiveMoveRequest.IsValid() || RequestID.GetID() != ActiveMoveRequest.GetID())
		return;

	UE_LOG(LogTemp, Verbose, TEXT("[DestinationMarker] Move finished: %s"), *Result.ToString());
	ClearTrackedDestination();
}

void UPlayerMode_World::UpdateDestinationState()
{
	if (!ActiveMoveRequest.IsValid())
		return;

	if (!PC || !TrackedMovePawn.IsValid() || TrackedMovePawn.Get() != PC->GetPawn() || !TrackedPathFollowing.IsValid()
		|| TrackedPathFollowing->GetStatus() == EPathFollowingStatus::Idle
		|| TrackedPathFollowing->GetCurrentRequestId().GetID() != ActiveMoveRequest.GetID())
		ClearTrackedDestination();
}

void UPlayerMode_World::ClearTrackedDestination()
{
	ActiveMoveRequest = FAIRequestID::InvalidRequest;
	TrackedMovePawn.Reset();
	if (WorldMapMarkerUI.IsValid())
		WorldMapMarkerUI->ClearDestination();
}

void UPlayerMode_World::HandleMouseWheel(const FInputActionValue& Value)
{
	if (CanProcessWorldInput() && IsValid(WanderingCameraManager))
		WanderingCameraManager->RequestZoom(Value.Get<float>());
}

void UPlayerMode_World::HandleCPressedKey(const FInputActionValue& Value)
{
	if (CanProcessWorldInput() && IsValid(WanderingCameraManager))
		WanderingCameraManager->FocusPlayer();
}

void UPlayerMode_World::CreateWanderingCamera()
{
	if (!PC || !PC->GetWorld() || IsValid(WanderingCameraManager))
		return;

	UClass* CameraClass = WanderingCameraManagerClass ? WanderingCameraManagerClass.Get() : AWanderingCameraManager::StaticClass();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PC;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WanderingCameraManager = PC->GetWorld()->SpawnActor<AWanderingCameraManager>(CameraClass, FTransform::Identity, SpawnParameters);

	if (!WanderingCameraManager)
		UE_LOG(LogTemp, Error, TEXT("[WorldCamera] Failed to spawn WanderingCameraManager."));
}

void UPlayerMode_World::UpdateWanderingCamera()
{
	if (!PC || !IsValid(WanderingCameraManager))
		return;

	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn != WanderingCameraManager->GetFollowTarget())
	{
		WanderingCameraManager->InitializeCamera(PC, PlayerPawn);

		if (PlayerPawn)
			PC->SetViewTarget(WanderingCameraManager);
	}

	if (UWorld* World = PC->GetWorld())
		WanderingCameraManager->UpdateCamera(World->GetDeltaSeconds(), CanProcessWorldInput());
}

bool UPlayerMode_World::CanProcessWorldInput() const
{
	if (!PC || !PC->IsLocalController() || !PC->GetPawn() || PC->IsMoveInputIgnored() || PC->IsLookInputIgnored())
		return false;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
		return false;

	if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
	{
		if (!ActionRouter->CanProcessNormalGameInput())
			return false;
	}

	if (LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport && !LocalPlayer->ViewportClient->Viewport->IsForegroundWindow())
		return false;

	if (UDialogueSubsystem* DialogueSubsystem = UDialogueSubsystem::Get(PC))
	{
		if (DialogueSubsystem->IsDialogueActive())
			return false;
	}

	return true;
}

bool UPlayerMode_World::TryGetCursorDestination(FVector& OutLocation, FVector& OutMarkerPosition) const
{
	OutLocation = FVector::ZeroVector;
	OutMarkerPosition = FVector::ZeroVector;
	if (!PC)
		return false;

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (WorldMapMarkerUI.IsValid() && PC->GetMousePosition(MouseX, MouseY))
	{
		AInteractionPointBase* Point = WorldMapMarkerUI->FindInteractionAtScreenPosition(FVector2D(MouseX, MouseY));
		if (TryGetInteractionDestination(Point, OutLocation, OutMarkerPosition))
			return true;
	}

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return false;

	for (AActor* HitActor = Hit.GetActor(); IsValid(HitActor); HitActor = HitActor->GetAttachParentActor())
	{
		if (TryGetInteractionDestination(Cast<AInteractionPointBase>(HitActor), OutLocation, OutMarkerPosition))
			return true;
	}

	if (WorldMapMarkerUI.IsValid())
	{
		AInteractionPointBase* Point = WorldMapMarkerUI->FindInteractionNearLocation(Hit.Location);
		if (TryGetInteractionDestination(Point, OutLocation, OutMarkerPosition))
			return true;
	}

	OutLocation = Hit.Location;
	OutMarkerPosition = Hit.Location;
	return true;
}

bool UPlayerMode_World::TryGetInteractionDestination(AInteractionPointBase* Point, FVector& OutLocation, FVector& OutMarkerPosition) const
{
	if (!IsValid(Point))
		return false;

	const UWorldMapMarkerComponent* Marker = Point->GetWorldMapMarkerComponent();
	if (!IsValid(Marker) || !Marker->CanSnapDestination())
		return false;

	const FVector Destination = Point->GetActorLocation();
	const FVector MarkerPosition = Marker->GetComponentLocation();
	if (Destination.ContainsNaN() || MarkerPosition.ContainsNaN())
		return false;

	OutLocation = Destination;
	OutMarkerPosition = MarkerPosition;
	return true;
}

void UPlayerMode_World::HandleInteract(AMuksiWorldCharacter* WorldCharacter)
{
	if (!WorldCharacter)
		return;

	AInteractionPointBase* InteractionTarget = WorldCharacter->GetCurrentInteractionTarget();

	UE_LOG(LogTemp, Verbose, TEXT("[Interaction] PlayerMode=%s Target=%s Character=%s"), *GetNameSafe(this), *GetNameSafe(InteractionTarget), *GetNameSafe(WorldCharacter));

	if (InteractionTarget)
		InteractionTarget->Interact(WorldCharacter);
}

void UPlayerMode_World::HandleOpenInventoryEquipment(AMuksiWorldCharacter* WorldCharacter)
{
	OpenInventoryEquipmentFromWorld();
}

void UPlayerMode_World::OpenInventoryEquipmentFromWorld()
{
	if (AWorldUIManager* WorldUIManager = FindWorldUIManager())
		WorldUIManager->OpenInventoryEquipmentUI();
}

AWorldUIManager* UPlayerMode_World::FindWorldUIManager() const
{
	UWorld* World = PC ? PC->GetWorld() : nullptr;
	if (!World)
		return nullptr;

	AWorldUIManager* WorldUIManager = Cast<AWorldUIManager>(UGameplayStatics::GetActorOfClass(World, AWorldUIManager::StaticClass()));
	if (!WorldUIManager)
		UE_LOG(LogTemp, Warning, TEXT("WorldUIManager not found in current level"));

	return WorldUIManager;
}
