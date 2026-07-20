#include "Muksi/Contents/Battle/Sequence/Executions/Damage/DamageExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Sequence/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Sequence/Executions/HitReaction/HitReactionExecution.h"

void UDamageExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FDamageExecutionData* DamageData = Context.GetExecutionData<FDamageExecutionData>();

	if (!DamageData || !Context.BattleGridManager)
	{
		FinishExecution(OnFinished);
		return;
	}

	for (const FIntPoint& TargetPoint : Context.TargetPoints)
	{
		const FBattleGridCell* Cell = Context.BattleGridManager->GetCell(TargetPoint);

		if (!Cell || !Cell->OccupyingActor)
		{
			continue;
		}

		ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get());

		if (!TargetCharacter || TargetCharacter == Context.Attacker)
		{
			continue;
		}

		ApplyDamageToTarget(Context, *DamageData, TargetCharacter);
	}

	FinishExecution(OnFinished);
}

void UDamageExecution::ApplyDamageToTarget(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter) const
{
	const int32 NewHP = FMath::Max(0, TargetCharacter->GetCurrentHP() - DamageData.DamageValue);
	TargetCharacter->SetCurrentHP(NewHP);

	UE_LOG(LogTemp, Log, TEXT("[DamageExecution] Damage=%d Target=%s NewHP=%d"), DamageData.DamageValue, *GetNameSafe(TargetCharacter), NewHP);

	if (DamageData.bTriggerHitReaction)
	{
		RequestHitReaction(Context, TargetCharacter);
	}
}

void UDamageExecution::RequestHitReaction(const FBattleExecutionContext& Context, ABattleCharacterBase* TargetCharacter) const
{
	if (!Context.CanRequestRuntimeExecutionChain() || !TargetCharacter)
	{
		return;
	}

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();

	TArray<FBattleExecutionEntry> HitReactionExecutions;
	HitReactionExecutions.Add(MoveTemp(HitReactionEntry));

	FBattleExecutionContext HitReactionContext = Context;
	HitReactionContext.ExecutionTarget = TargetCharacter;
	HitReactionContext.ExecutionData.Reset();

	Context.RequestRuntimeExecutionChain.Execute(HitReactionExecutions, HitReactionContext);
}

const UScriptStruct* UDamageExecution::GetExecutionDataStruct() const
{
	return FDamageExecutionData::StaticStruct();
}