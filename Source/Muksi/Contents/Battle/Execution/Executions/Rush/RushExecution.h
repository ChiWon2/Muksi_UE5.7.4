#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Rush/RushExecutionData.h"
#include "RushExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleGridNavigationComponent;
class UMuksiBattleAnimationComponent;
class UMuksiBattleMovementComponent;
class UAnimMontage;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API URushExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	bool BuildRushPath(const FTargetingStepResult& StepResult, TArray<FHexOffsetCoord>& OutGridPath);
	bool CommitGridMovement();

	void HandleMovementFinished(bool bInterrupted);
	void RestoreStartWorldLocation();
	void RequestMovementEndAnimation();
	void FinishRushExecution(bool bRequestEndAnimation = false);

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

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> RushMontage = nullptr;

	FRushExecutionData CachedRushData;

	FHexOffsetCoord StartCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	FHexOffsetCoord DestinationCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	FBattleExecutionFinished CachedOnFinished;
};
