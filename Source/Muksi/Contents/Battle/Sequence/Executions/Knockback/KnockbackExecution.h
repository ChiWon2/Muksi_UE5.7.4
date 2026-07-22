#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridCoord.h"
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
	bool FindActorGridCoord(const ABattleGridManager* GridManager, const AActor* Actor, FIntPoint& OutCoord) const;
	bool FindKnockbackDirection(const ABattleGridManager* GridManager, const FIntPoint& AttackerCoord, const FIntPoint& TargetCoord, FCubeCoord& OutDirection) const;
	FIntPoint GetNextCoord(const ABattleGridManager* GridManager, const FIntPoint& CurrentCoord, const FCubeCoord& Direction) const;
	void HandleMovementFinished(bool bInterrupted);
	void CompleteExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> CachedGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> KnockbackTarget = nullptr;

	FIntPoint StartCoord = FIntPoint::ZeroValue;
	FIntPoint DestinationCoord = FIntPoint::ZeroValue;
	FBattleExecutionFinished CachedOnFinished;
};