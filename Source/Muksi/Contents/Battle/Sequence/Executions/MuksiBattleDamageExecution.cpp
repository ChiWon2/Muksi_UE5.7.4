#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleDamageExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/ExecutionData/MuksiBattleDamageExecutionData.h"
#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleHitReactionExecution.h"

void UMuksiBattleDamageExecution::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	const FMuksiBattleDamageExecutionData* DamageData = Context.GetExecutionData<FMuksiBattleDamageExecutionData>();

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

void UMuksiBattleDamageExecution::ApplyDamageToTarget(const FMuksiBattleExecutionContext& Context, const FMuksiBattleDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter) const
{
	const int32 NewHP = FMath::Max(0, TargetCharacter->GetCurrentHP() - DamageData.DamageValue);
	TargetCharacter->SetCurrentHP(NewHP);

	//BattleCharacter-> ApplyDamage()

	UE_LOG(LogTemp, Log, TEXT("[DamageExecution] Damage=%d Target=%s NewHP=%d"), DamageData.DamageValue, *GetNameSafe(TargetCharacter), NewHP);

	if (DamageData.bTriggerHitReaction)
	{
		RequestHitReaction(Context, TargetCharacter);
	}
}

void UMuksiBattleDamageExecution::RequestHitReaction(const FMuksiBattleExecutionContext& Context, ABattleCharacterBase* TargetCharacter) const
{
	if (!Context.CanRequestSystemExecution() || !TargetCharacter)
	{
		return;
	}

	FMuksiBattleExecutionContext HitReactionContext = Context;
	HitReactionContext.TargetCharacter = TargetCharacter;
	HitReactionContext.ExecutionData.Reset();

	Context.RequestSystemExecution.Execute(UMuksiBattleHitReactionExecution::StaticClass(), HitReactionContext);
}

const UScriptStruct* UMuksiBattleDamageExecution::GetExecutionDataStruct() const
{
	return FMuksiBattleDamageExecutionData::StaticStruct();
}
