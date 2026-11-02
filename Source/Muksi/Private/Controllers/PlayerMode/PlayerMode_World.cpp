#include "Controllers/PlayerMode/PlayerMode_World.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/Travel/Public/Camera/WanderingCameraManager.h"
#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"
#include "Muksi/Dialogue/DialogueSubsystem.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Kismet/GameplayStatics.h"
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
	if (TryGetCursorHitLocation(HitLocation))
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, HitLocation);
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

bool UPlayerMode_World::TryGetCursorHitLocation(FVector& OutLocation) const
{
	if (!PC)
		return false;

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return false;

	OutLocation = Hit.Location;
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
