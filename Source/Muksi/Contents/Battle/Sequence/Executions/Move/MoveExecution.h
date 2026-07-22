#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"
#include "Muksi/Contents/Battle/Sequence/Executions/Move/MoveExecutionData.h"
#include "MoveExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleGridNavigationComponent;
class UMuksiBattleAnimationComponent;
class UMuksiBattleMovementComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMoveExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
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

	FMoveExecutionData CachedMoveData;

	FIntPoint StartCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
	FIntPoint DestinationCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	FBattleExecutionFinished CachedOnFinished;

	bool bHasMoveData = false;
};