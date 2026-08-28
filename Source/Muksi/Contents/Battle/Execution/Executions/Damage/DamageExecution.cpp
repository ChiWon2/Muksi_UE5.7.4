#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/BattleCardEffectTypes.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

namespace
{
	struct FPendingHitResponse
	{
		TArray<FBattleExecutionEntry> Executions;
		FBattleExecutionContext Context;
	};
}

void UDamageExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = MoveTemp(OnFinished);

	const FDamageExecutionData* DamageData = Context.GetExecutionData<FDamageExecutionData>();

	if (!DamageData)
	{
		CompleteDamageExecution();
		return;
	}

	TArray<ABattleCharacterBase*> TargetCharacters;
	CollectTargets(Context, *DamageData, TargetCharacters);

	TArray<FPendingHitResponse> PendingHitResponses;

	for (ABattleCharacterBase* TargetCharacter : TargetCharacters)
	{
		ApplyDamageToTarget(Context, *DamageData, TargetCharacter);

		FPendingHitResponse HitResponse;
		HitResponse.Context = Context;
		HitResponse.Context.ExecutionTarget = TargetCharacter;
		HitResponse.Context.ExecutionData.Reset();
		BuildHitResponseExecutions(HitResponse.Context, *DamageData, TargetCharacter, HitResponse.Executions);

		if (!HitResponse.Executions.IsEmpty()) PendingHitResponses.Add(MoveTemp(HitResponse));
	}

	for (const FPendingHitResponse& HitResponse : PendingHitResponses)
	{
		if (!Context.CanRequestRuntimeExecutionChain())
		{
			continue;
		}

		Context.RequestRuntimeExecutionChain.Execute(
			HitResponse.Executions,
			HitResponse.Context,
			FSimpleDelegate());
	}

	CompleteDamageExecution();
}

void UDamageExecution::CollectTargets(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, TArray<ABattleCharacterBase*>& OutTargets) const
{
	OutTargets.Reset();

	switch (DamageData.TargetPolicy)
	{
	case EDamageExecutionTargetPolicy::ExecutionTarget:
		if (Context.ExecutionTarget) OutTargets.Add(Context.ExecutionTarget);
		return;
	case EDamageExecutionTargetPolicy::Attacker:
		if (Context.Attacker) OutTargets.Add(Context.Attacker);
		return;
	case EDamageExecutionTargetPolicy::TargetingResult:
	default:
		break;
	}

	if (!Context.BattleGridManager)
	{
		return;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();
	if (!StepResult) return;

	for (ABattleCharacterBase* TargetCharacter : StepResult->Targets)
	{
		if (TargetCharacter && TargetCharacter != Context.Attacker) OutTargets.AddUnique(TargetCharacter);
	}

}

void UDamageExecution::ApplyDamageToTarget(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter) const
{
	const int32 NewHP = FMath::Max(0, TargetCharacter->GetCurrentHP() - DamageData.DamageValue);
	TargetCharacter->SetCurrentHP(NewHP);

	UE_LOG(LogTemp, Log, TEXT("[DamageExecution] Damage=%d Target=%s NewHP=%d"), DamageData.DamageValue, *GetNameSafe(TargetCharacter), NewHP);

}

void UDamageExecution::BuildHitResponseExecutions(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter, TArray<FBattleExecutionEntry>& OutExecutions) const
{
	if (!TargetCharacter)
	{
		return;
	}

	if (DamageData.bTriggerHitReaction)
	{
		FBattleExecutionEntry HitReactionEntry;
		HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
		OutExecutions.Add(MoveTemp(HitReactionEntry));
	}

	if (Context.ExecutionMode != EBattleExecutionMode::Sequence)
	{
		return;
	}

	if (!DamageData.bTriggerStatusEffectReactions)
	{
		return;
	}

	if (Context.Attacker)
	{
		if (UMuksiStatusEffectComponent* AttackerStatusEffects = Context.Attacker->GetStatusEffectComponent())
		{
			AttackerStatusEffects->AppendHitDealtExecutions(Context, DamageData.DamageValue, OutExecutions);
		}
	}

	if (UMuksiStatusEffectComponent* TargetStatusEffects = TargetCharacter->GetStatusEffectComponent())
	{
		TargetStatusEffects->AppendHitReceivedExecutions(Context, DamageData.DamageValue, OutExecutions);
	}
}

void UDamageExecution::CompleteDamageExecution()
{
	if (IsExecutionFinished())
	{
		return;
	}

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UDamageExecution::GetExecutionDataStruct() const
{
	return FDamageExecutionData::StaticStruct();
}
