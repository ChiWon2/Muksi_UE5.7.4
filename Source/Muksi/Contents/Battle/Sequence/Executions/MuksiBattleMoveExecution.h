#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleMoveExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleGridNavigationComponent;
class UMuksiBattleMovementComponent;
class UMuksiBattleAnimationComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleMoveExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(
		const FMuksiBattleExecutionContext& Context,
		FMuksiBattleExecutionFinished OnFinished
	) override;

private:
	void StartTeleportMovement();
	void StartJumpMovement();
	void StartGroundPathMovement();

	bool ValidateDestination() const;
	bool CommitGridMovement();

	void HandleMovementFinished(bool bInterrupted);
	void RestoreStartWorldLocation();
	void RequestMovementEndAnimation();
	void FinishMoveExecution(bool bRequestEndAnimation = false);

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> MovingCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleGridNavigationComponent> NavigationComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> MovementComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> AnimationComponent = nullptr;

	FIntPoint StartCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
	FIntPoint DestinationCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	EMuksiBattleMoveType CachedMoveType = EMuksiBattleMoveType::None;
	FMuksiBattleExecutionFinished CachedOnFinished;

	bool bExecutionFinished = false;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Movement|Ground")
	float GroundMoveSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Movement|Ground|Animation")
	FName GroundPathEndSection = TEXT("Run_End");

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Movement|Jump")
	float JumpDuration = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Movement|Jump")
	float JumpArcHeight = 200.0f;
};
