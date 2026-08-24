#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"
#include "KnockbackExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UKnockbackExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	bool FindActorGridCoord(ABattleGridManager* GridManager, const AActor* Actor, FHexOffsetCoord& OutCoord);
	bool FindKnockbackDirection(const ABattleGridManager* GridManager, const FHexOffsetCoord& AttackerCoord, const FHexOffsetCoord& TargetCoord, FHexCubeCoord& OutDirection) const;
	FHexOffsetCoord GetNextCoord(const ABattleGridManager* GridManager, const FHexOffsetCoord& CurrentCoord, const FHexCubeCoord& Direction) const;
	void HandleMovementFinished(bool bInterrupted);
	void CompleteExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> CachedGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> KnockbackTarget = nullptr;

	FHexOffsetCoord StartCoord = FHexOffsetCoord();
	FHexOffsetCoord DestinationCoord = FHexOffsetCoord();
	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
	FBattleExecutionFinished CachedOnFinished;
};
