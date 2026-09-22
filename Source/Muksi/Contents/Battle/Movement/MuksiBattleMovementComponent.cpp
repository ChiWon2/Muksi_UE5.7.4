#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

#include "GameFramework/Actor.h"

UMuksiBattleMovementComponent::UMuksiBattleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMuksiBattleMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UMuksiBattleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (MovementMode)
	{
	case EMuksiBattleMovementMode::Rotation:
		UpdateRotationMovement(DeltaTime);
		break;

	case EMuksiBattleMovementMode::Arc:
		UpdateArcMovement(DeltaTime);
		break;

	case EMuksiBattleMovementMode::Path:
		UpdatePathMovement(DeltaTime);
		break;

	case EMuksiBattleMovementMode::Linear:
		UpdateLinearMovement(DeltaTime);
		break;

	case EMuksiBattleMovementMode::None:
	default:
		break;
	}
}

void UMuksiBattleMovementComponent::StartRotateTowardLocation(const FVector& TargetWorldLocation, float RotationSpeed, FMuksiBattleMovementFinished OnFinished)
{
	if (IsMoving())
	{
		StopMovement(true);
	}

	AActor* Owner = GetOwner();

	if (!Owner || RotationSpeed <= KINDA_SMALL_NUMBER)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	FVector Direction = TargetWorldLocation - Owner->GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		OnFinished.ExecuteIfBound(false);
		return;
	}

	MovementMode = EMuksiBattleMovementMode::Rotation;
	CachedOnFinished = OnFinished;
	RotationTargetLocation = TargetWorldLocation;
	CurrentRotationSpeed = RotationSpeed;

	SetComponentTickEnabled(true);
}

void UMuksiBattleMovementComponent::StartTeleportMove(const FVector& TargetWorldLocation, FMuksiBattleMovementFinished OnFinished)
{
	if (IsMoving())
	{
		StopMovement(true);
	}

	AActor* Owner = GetOwner();

	if (!Owner)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	Owner->SetActorLocation(TargetWorldLocation);
	OnFinished.ExecuteIfBound(false);
}

void UMuksiBattleMovementComponent::StartArcMove(const FVector& TargetWorldLocation, float Duration, float ArcHeight, FMuksiBattleMovementFinished OnFinished)
{
	if (IsMoving())
	{
		StopMovement(true);
	}

	AActor* Owner = GetOwner();

	if (!Owner || Duration <= KINDA_SMALL_NUMBER)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	MovementMode = EMuksiBattleMovementMode::Arc;
	CachedOnFinished = OnFinished;
	ArcStartLocation = Owner->GetActorLocation();
	ArcTargetLocation = TargetWorldLocation;
	ArcDuration = Duration;
	ArcElapsedTime = 0.0f;
	CurrentArcHeight = FMath::Max(0.0f, ArcHeight);

	SetComponentTickEnabled(true);
}

void UMuksiBattleMovementComponent::StartPathMove(const TArray<FVector>& WorldPath, float MoveSpeed, FMuksiBattleMovementFinished OnFinished, bool bRotateTowardPath)
{
	if (IsMoving())
	{
		StopMovement(true);
	}

	AActor* Owner = GetOwner();

	if (!Owner)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	if (WorldPath.IsEmpty())
	{
		OnFinished.ExecuteIfBound(false);
		return;
	}

	if (MoveSpeed <= KINDA_SMALL_NUMBER)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	MovementMode = EMuksiBattleMovementMode::Path;
	CachedOnFinished = OnFinished;
	CurrentWorldPath = WorldPath;
	CurrentPathIndex = 0;
	CurrentMoveSpeed = MoveSpeed;
	bCurrentPathRotationEnabled = bRotateTowardPath;

	SetComponentTickEnabled(true);
}

void UMuksiBattleMovementComponent::StartLinearMove(const FVector& TargetWorldLocation, float Duration, FMuksiBattleMovementFinished OnFinished)
{
	if (IsMoving())
		StopMovement(true);

	AActor* Owner = GetOwner();

	if (!Owner)
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	if (Duration <= KINDA_SMALL_NUMBER)
	{
		Owner->SetActorLocation(TargetWorldLocation);
		OnFinished.ExecuteIfBound(false);
		return;
	}

	MovementMode = EMuksiBattleMovementMode::Linear;
	CachedOnFinished = OnFinished;
	LinearStartLocation = Owner->GetActorLocation();
	LinearTargetLocation = TargetWorldLocation;
	LinearDuration = Duration;
	LinearElapsedTime = 0.0f;

	SetComponentTickEnabled(true);
}

void UMuksiBattleMovementComponent::SavePresentationTransform()
{
	if (bHasSavedPresentationTransform)
		return;

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	SavedPresentationTransform = Owner->GetActorTransform();
	bHasSavedPresentationTransform = true;
}

void UMuksiBattleMovementComponent::ClearSavedPresentationTransform()
{
	SavedPresentationTransform = FTransform::Identity;
	bHasSavedPresentationTransform = false;
}

void UMuksiBattleMovementComponent::StopMovement(bool bNotifyInterruption)
{
	if (!IsMoving())
	{
		return;
	}

	if (bNotifyInterruption)
	{
		FinishMovement(true);
		return;
	}

	ResetMovementState();
}

void UMuksiBattleMovementComponent::UpdateRotationMovement(float DeltaTime)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		FinishMovement(true);
		return;
	}

	FVector Direction = RotationTargetLocation - Owner->GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		FinishMovement(false);
		return;
	}

	const float TargetYaw = FRotator::NormalizeAxis(Direction.Rotation().Yaw + MovementYawOffset);
	const FRotator CurrentRotation = Owner->GetActorRotation();
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetYaw);

	if (FMath::Abs(DeltaYaw) <= RotationTolerance)
	{
		Owner->SetActorRotation(FRotator(0.0f, TargetYaw, 0.0f));
		FinishMovement(false);
		return;
	}

	const float MaxYawStep = CurrentRotationSpeed * DeltaTime;
	const float AppliedYawStep = FMath::Clamp(DeltaYaw, -MaxYawStep, MaxYawStep);
	const float NewYaw = FRotator::NormalizeAxis(CurrentRotation.Yaw + AppliedYawStep);
	const FRotator NewRotation(0.0f, NewYaw, 0.0f);
	const bool bRotationApplied = Owner->SetActorRotation(NewRotation);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleMovement][Rotation] Owner=%s Current=%s New=%s Applied=%s Root=%s Mobility=%d"),
		*GetNameSafe(Owner),
		*CurrentRotation.ToString(),
		*NewRotation.ToString(),
		bRotationApplied ? TEXT("true") : TEXT("false"),
		*GetNameSafe(Owner->GetRootComponent()),
		Owner->GetRootComponent() ? static_cast<int32>(Owner->GetRootComponent()->Mobility) : -1
	);
}

void UMuksiBattleMovementComponent::UpdateArcMovement(float DeltaTime)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		FinishMovement(true);
		return;
	}

	ArcElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(ArcElapsedTime / ArcDuration, 0.0f, 1.0f);
	FVector NewLocation = FMath::Lerp(ArcStartLocation, ArcTargetLocation, Alpha);
	const float HeightOffset = 4.0f * CurrentArcHeight * Alpha * (1.0f - Alpha);

	NewLocation.Z += HeightOffset;
	Owner->SetActorLocation(NewLocation);

	if (Alpha < 1.0f)
	{
		return;
	}

	Owner->SetActorLocation(ArcTargetLocation);
	FinishMovement(false);
}

void UMuksiBattleMovementComponent::UpdatePathMovement(float DeltaTime)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		FinishMovement(true);
		return;
	}

	if (!CurrentWorldPath.IsValidIndex(CurrentPathIndex))
	{
		FinishMovement(false);
		return;
	}

	float RemainingMoveDistance = CurrentMoveSpeed * DeltaTime;

	while (RemainingMoveDistance > KINDA_SMALL_NUMBER && CurrentWorldPath.IsValidIndex(CurrentPathIndex))
	{
		const FVector CurrentLocation = Owner->GetActorLocation();
		const FVector TargetLocation = CurrentWorldPath[CurrentPathIndex];
		const FVector ToTarget = TargetLocation - CurrentLocation;
		const float DistanceToTarget = ToTarget.Size();

		if (DistanceToTarget <= KINDA_SMALL_NUMBER)
		{
			Owner->SetActorLocation(TargetLocation);
			++CurrentPathIndex;
			continue;
		}

		if (bCurrentPathRotationEnabled)
		{
			RotateOwnerToward(ToTarget, DeltaTime);
		}
		if (RemainingMoveDistance >= DistanceToTarget)
		{
			Owner->SetActorLocation(TargetLocation);
			RemainingMoveDistance -= DistanceToTarget;
			++CurrentPathIndex;
			continue;
		}

		const FVector MoveDirection = ToTarget / DistanceToTarget;
		const FVector NewLocation = CurrentLocation + MoveDirection * RemainingMoveDistance;

		Owner->SetActorLocation(NewLocation);
		RemainingMoveDistance = 0.0f;
	}

	if (!CurrentWorldPath.IsValidIndex(CurrentPathIndex))
	{
		FinishMovement(false);
	}
}


void UMuksiBattleMovementComponent::UpdateLinearMovement(float DeltaTime)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		FinishMovement(true);
		return;
	}

	LinearElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(LinearElapsedTime / LinearDuration, 0.0f, 1.0f);
	Owner->SetActorLocation(FMath::Lerp(LinearStartLocation, LinearTargetLocation, Alpha));

	if (Alpha < 1.0f)
		return;

	Owner->SetActorLocation(LinearTargetLocation);
	FinishMovement(false);
}

void UMuksiBattleMovementComponent::FinishMovement(bool bInterrupted)
{
	FMuksiBattleMovementFinished FinishedDelegate = CachedOnFinished;

	ResetMovementState();
	FinishedDelegate.ExecuteIfBound(bInterrupted);
}

void UMuksiBattleMovementComponent::ResetMovementState()
{
	SetComponentTickEnabled(false);

	MovementMode = EMuksiBattleMovementMode::None;
	CachedOnFinished.Unbind();

	RotationTargetLocation = FVector::ZeroVector;
	CurrentRotationSpeed = 0.0f;

	bCurrentPathRotationEnabled = true;

	ArcStartLocation = FVector::ZeroVector;
	ArcTargetLocation = FVector::ZeroVector;
	ArcDuration = 0.0f;
	ArcElapsedTime = 0.0f;
	CurrentArcHeight = 0.0f;

	CurrentWorldPath.Empty();
	CurrentPathIndex = INDEX_NONE;
	CurrentMoveSpeed = 0.0f;

	LinearStartLocation = FVector::ZeroVector;
	LinearTargetLocation = FVector::ZeroVector;
	LinearDuration = 0.0f;
	LinearElapsedTime = 0.0f;
}

void UMuksiBattleMovementComponent::RotateOwnerToward(const FVector& Direction, float DeltaTime) const
{
	if (!bRotateTowardMovementDirection)
		return;

	AActor* Owner = GetOwner();

	if (!Owner)
		return;

	FVector HorizontalDirection = Direction;
	HorizontalDirection.Z = 0.0f;

	if (HorizontalDirection.IsNearlyZero())
		return;

	const float TargetYaw = FRotator::NormalizeAxis(HorizontalDirection.Rotation().Yaw + MovementYawOffset);
	const FRotator CurrentRotation = Owner->GetActorRotation();
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetYaw);
	const float MaxYawStep = PathRotationSpeed * DeltaTime;
	const float AppliedYawStep = FMath::Clamp(DeltaYaw, -MaxYawStep, MaxYawStep);
	const float NewYaw = FRotator::NormalizeAxis(CurrentRotation.Yaw + AppliedYawStep);
	const FRotator NewRotation(0.0f, NewYaw, 0.0f);
	const bool bRotationApplied = Owner->SetActorRotation(NewRotation);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleMovement][PathRotation] Owner=%s Current=%s New=%s Applied=%s Root=%s Mobility=%d"),
		*GetNameSafe(Owner),
		*CurrentRotation.ToString(),
		*NewRotation.ToString(),
		bRotationApplied ? TEXT("true") : TEXT("false"),
		*GetNameSafe(Owner->GetRootComponent()),
		Owner->GetRootComponent() ? static_cast<int32>(Owner->GetRootComponent()->Mobility) : -1
	);
}
