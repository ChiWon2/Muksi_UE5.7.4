#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/ExecutionData/MuksiBattleMoveExecutionData.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleMoveExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleGridNavigationComponent;
class UMuksiBattleAnimationComponent;
class UMuksiBattleMovementComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleMoveExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

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

	FMuksiBattleMoveExecutionData CachedMoveData;

	FIntPoint StartCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
	FIntPoint DestinationCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	FMuksiBattleExecutionFinished CachedOnFinished;

	bool bHasMoveData = false;
	bool bExecutionFinished = false;
};
