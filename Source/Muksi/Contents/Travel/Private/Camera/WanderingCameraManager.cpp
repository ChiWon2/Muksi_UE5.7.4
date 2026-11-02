#include "Muksi/Contents/Travel/Public/Camera/WanderingCameraManager.h"

#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

AWanderingCameraManager::AWanderingCameraManager()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SceneRoot);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->SetRelativeRotation(FRotator(NearPitch, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void AWanderingCameraManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateCameraPitch();
}

void AWanderingCameraManager::InitializeCamera(APlayerController* PlayerController, APawn* PlayerPawn)
{
	OwningPlayerController = PlayerController;
	FollowTarget = PlayerPawn;
	EndDrag();
	bFollowingPlayer = true;
	bReturningToPlayer = false;

	if (!PlayerPawn)
		return;

	SetActorLocation(PlayerPawn->GetActorLocation());

	if (const AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(PlayerPawn))
	{
		if (const USpringArmComponent* CharacterBoom = WorldCharacter->GetCameraBoom())
			CameraBoom->TargetArmLength = ClampZoomDistance(CharacterBoom->TargetArmLength);

		if (const UCameraComponent* CharacterCamera = WorldCharacter->GetTopDownCameraComponent())
			Camera->SetFieldOfView(CharacterCamera->FieldOfView);
	}

	TargetArmLength = ClampZoomDistance(CameraBoom->TargetArmLength);
	CameraBoom->TargetArmLength = TargetArmLength;
	UpdateCameraPitch();
}

void AWanderingCameraManager::UpdateCamera(float DeltaTime, bool bAllowCameraInput)
{
	if (!bAllowCameraInput)
		EndDrag();

	if (!OwningPlayerController.IsValid() || !FollowTarget.IsValid())
	{
		EndDrag();
		return;
	}

	if (bAllowCameraInput)
		UpdateDrag();

	if (bFollowingPlayer)
	{
		const FVector PlayerLocation = FollowTarget->GetActorLocation();
		const FVector CameraCenter = bReturningToPlayer ? FMath::VInterpTo(GetActorLocation(), PlayerLocation, DeltaTime, ReturnInterpSpeed) : PlayerLocation;
		SetActorLocation(CameraCenter);

		if (FVector::DistSquared(CameraCenter, PlayerLocation) <= 1.0f)
			bReturningToPlayer = false;
	}

	TargetArmLength = ClampZoomDistance(TargetArmLength);
	const float CurrentArmLength = ClampZoomDistance(CameraBoom->TargetArmLength);
	CameraBoom->TargetArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomInterpSpeed);
	UpdateCameraPitch();
}

void AWanderingCameraManager::BeginDrag()
{
	EndDrag();

	APlayerController* PlayerController = OwningPlayerController.Get();
	if (!PlayerController)
		return;

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
		return;

	DragStartScreenPosition = FVector2D(MouseX, MouseY);
	DragPlaneHeight = GetActorLocation().Z;
	PreviousDragScreenPosition = DragStartScreenPosition;
	bDragPressed = true;
}

void AWanderingCameraManager::EndDrag()
{
	bDragPressed = false;
	bDragging = false;
}

void AWanderingCameraManager::RequestZoom(float WheelValue)
{
	if (!FMath::IsFinite(WheelValue) || FMath::IsNearlyZero(WheelValue))
		return;

	const float SafeWheelValue = FMath::Clamp(WheelValue, -10.0f, 10.0f);
	const float ZoomMultiplier = FMath::Pow(1.0f + FMath::Clamp(ZoomStepRatio, 0.0f, 1.0f), -SafeWheelValue);
	TargetArmLength = ClampZoomDistance(TargetArmLength * ZoomMultiplier);
}

void AWanderingCameraManager::FocusPlayer()
{
	EndDrag();
	bFollowingPlayer = true;
	bReturningToPlayer = true;
}

float AWanderingCameraManager::GetZoomDistance() const
{
	return CameraBoom->TargetArmLength;
}

float AWanderingCameraManager::GetCameraPitch() const
{
	return CameraBoom->GetRelativeRotation().Pitch;
}

float AWanderingCameraManager::ClampZoomDistance(float Distance) const
{
	const float MinDistance = FMath::Max(MinZoomDistance, 1.0f);
	const float MaxDistance = FMath::Max(MaxZoomDistance, MinDistance);
	return FMath::Clamp(Distance, MinDistance, MaxDistance);
}

void AWanderingCameraManager::UpdateCameraPitch()
{
	const float StartDistance = FMath::Max(PitchTransitionStartDistance, 1.0f);
	const float EndDistance = FMath::Max(PitchTransitionEndDistance, StartDistance + 1.0f);
	const float CurrentDistance = CameraBoom->TargetArmLength;
	float TransitionAlpha = 0.0f;

	if (CurrentDistance >= EndDistance)
		TransitionAlpha = 1.0f;
	else if (CurrentDistance > StartDistance)
	{
		const float DistanceRatio = FMath::Loge(CurrentDistance / StartDistance) / FMath::Loge(EndDistance / StartDistance);
		TransitionAlpha = FMath::SmoothStep(0.0f, 1.0f, DistanceRatio);
	}

	const float ClampedNearPitch = FMath::Clamp(NearPitch, -90.0f, -1.0f);
	const float ClampedFarPitch = FMath::Clamp(FarPitch, -90.0f, -1.0f);
	FRotator CameraRotation = CameraBoom->GetRelativeRotation();
	CameraRotation.Pitch = FMath::Lerp(ClampedNearPitch, ClampedFarPitch, TransitionAlpha);
	CameraBoom->SetRelativeRotation(CameraRotation);
}

bool AWanderingCameraManager::ProjectScreenToDragPlane(const FVector2D& ScreenPosition, FVector& OutLocation) const
{
	APlayerController* PlayerController = OwningPlayerController.Get();
	if (!PlayerController)
		return false;

	FVector RayOrigin = FVector::ZeroVector;
	FVector RayDirection = FVector::ZeroVector;
	if (!PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, RayOrigin, RayDirection))
		return false;

	if (FMath::IsNearlyZero(RayDirection.Z))
		return false;

	const float RayDistance = (DragPlaneHeight - RayOrigin.Z) / RayDirection.Z;
	if (RayDistance < 0.0f || !FMath::IsFinite(RayDistance))
		return false;

	OutLocation = RayOrigin + RayDirection * RayDistance;
	return !OutLocation.ContainsNaN();
}

void AWanderingCameraManager::UpdateDrag()
{
	if (!bDragPressed)
		return;

	APlayerController* PlayerController = OwningPlayerController.Get();
	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController || !PlayerController->GetMousePosition(MouseX, MouseY))
	{
		EndDrag();
		return;
	}

	const FVector2D MousePosition(MouseX, MouseY);
	if (!bDragging)
	{
		const float Threshold = FMath::Max(DragThresholdPixels, 0.0f);
		if (FVector2D::DistSquared(MousePosition, DragStartScreenPosition) <= FMath::Square(Threshold))
			return;

		bDragging = true;
		bFollowingPlayer = false;
		bReturningToPlayer = false;
	}

	FVector PreviousWorldPosition = FVector::ZeroVector;
	FVector CurrentWorldPosition = FVector::ZeroVector;
	if (ProjectScreenToDragPlane(PreviousDragScreenPosition, PreviousWorldPosition) && ProjectScreenToDragPlane(MousePosition, CurrentWorldPosition))
		SetActorLocation(GetActorLocation() + PreviousWorldPosition - CurrentWorldPosition);

	PreviousDragScreenPosition = MousePosition;
}
