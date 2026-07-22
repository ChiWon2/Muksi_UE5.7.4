#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MuksiBattleMovementComponent.generated.h"

DECLARE_DELEGATE_OneParam(FMuksiBattleMovementFinished, bool);

enum class EMuksiBattleMovementMode : uint8
{
	None,
	Rotation,
	Arc,
	Path
};

UCLASS(ClassGroup = (Battle), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiBattleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMuksiBattleMovementComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void StartRotateTowardLocation(const FVector& TargetWorldLocation, float RotationSpeed, FMuksiBattleMovementFinished OnFinished);
	void StartTeleportMove(const FVector& TargetWorldLocation, FMuksiBattleMovementFinished OnFinished);
	void StartArcMove(const FVector& TargetWorldLocation, float Duration, float ArcHeight, FMuksiBattleMovementFinished OnFinished);
	void StartPathMove(const TArray<FVector>& WorldPath, float MoveSpeed, FMuksiBattleMovementFinished OnFinished, bool bRotateTowardPath = true);

	UFUNCTION(BlueprintCallable, Category = "Battle|Movement")
	void StopMovement(bool bNotifyInterruption = true);

	UFUNCTION(BlueprintPure, Category = "Battle|Movement")
	bool IsMoving() const { return MovementMode != EMuksiBattleMovementMode::None; }

private:
	void UpdateRotationMovement(float DeltaTime);
	void UpdateArcMovement(float DeltaTime);
	void UpdatePathMovement(float DeltaTime);
	void FinishMovement(bool bInterrupted);
	void ResetMovementState();
	void RotateOwnerToward(const FVector& Direction, float DeltaTime) const;

private:
	EMuksiBattleMovementMode MovementMode = EMuksiBattleMovementMode::None;
	FMuksiBattleMovementFinished CachedOnFinished;

	FVector RotationTargetLocation = FVector::ZeroVector;
	float CurrentRotationSpeed = 0.0f;

	FVector ArcStartLocation = FVector::ZeroVector;
	FVector ArcTargetLocation = FVector::ZeroVector;
	float ArcDuration = 0.0f;
	float ArcElapsedTime = 0.0f;
	float CurrentArcHeight = 0.0f;

	bool bCurrentPathRotationEnabled = true;

	TArray<FVector> CurrentWorldPath;
	int32 CurrentPathIndex = INDEX_NONE;
	float CurrentMoveSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Battle|Movement|Rotation")
	bool bRotateTowardMovementDirection = true;

	UPROPERTY(EditAnywhere, Category = "Battle|Movement|Rotation", meta = (ClampMin = "0.0"))
	float PathRotationSpeed = 720.0f;

	UPROPERTY(EditAnywhere, Category = "Battle|Movement|Rotation", meta = (ClampMin = "0.0"))
	float RotationTolerance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Battle|Movement|Rotation")
	float MovementYawOffset = 0.0f;
};