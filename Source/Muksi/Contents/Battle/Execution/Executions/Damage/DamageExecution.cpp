#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

namespace
{
	struct FPendingHitResponse
	{
		TArray<FBattleExecutionEntry> ExecutionEntries;
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
		FPendingHitResponse HitResponse;
		HitResponse.Context = Context;
		HitResponse.Context.ExecutionTarget = TargetCharacter;
		HitResponse.Context.ExecutionData.Reset();

		FName HitReactionAnimKey = NAME_None;

		const int32 AppliedDamage = ApplyDamageToTarget(Context, *DamageData, TargetCharacter, HitReactionAnimKey);
		BuildHitResponseExecutionEntries(HitResponse.Context, *DamageData, TargetCharacter, AppliedDamage, HitReactionAnimKey, HitResponse.ExecutionEntries);

		if (!HitResponse.ExecutionEntries.IsEmpty())
			PendingHitResponses.Add(MoveTemp(HitResponse));
	}

	for (const FPendingHitResponse& HitResponse : PendingHitResponses)
	{
		if (!Context.CanRequestRuntimeExecutionEntries())
			continue;

		Context.RequestRuntimeExecutionEntries.Execute(
			HitResponse.ExecutionEntries,
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
	case EBattleExecutionTargetPolicy::ExecutionTarget:
		if (Context.ExecutionTarget)
			OutTargets.Add(Context.ExecutionTarget);
		return;
	case EBattleExecutionTargetPolicy::Attacker:
		if (Context.Attacker)
			OutTargets.Add(Context.Attacker);
		return;
	case EBattleExecutionTargetPolicy::TargetingResult:
	default:
		break;
	}

	if (!Context.BattleGridManager)
		return;

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();
	if (!StepResult)
		return;

	for (ABattleCharacterBase* TargetCharacter : StepResult->Targets)
	{
		if (TargetCharacter && TargetCharacter != Context.Attacker)
			OutTargets.AddUnique(TargetCharacter);
	}
}

int32 UDamageExecution::ApplyDamageToTarget(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter, FName& OutHitReactionAnimKey) const
{
	if (!TargetCharacter || DamageData.DamageValue <= 0)
		return 0;

	int32 FinalDamage = DamageData.DamageValue;

	if (Context.ExecutionMode == EBattleExecutionMode::ActualBattle && DamageData.DefensePolicy == EDamageDefensePolicy::ApplyDefense)
	{
		if (UMuksiStatusEffectComponent* StatusEffectComponent = TargetCharacter->GetStatusEffectComponent())
			FinalDamage = StatusEffectComponent->ApplyIncomingDamageModifiers(FinalDamage, OutHitReactionAnimKey);
	}

	if (FinalDamage <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[DamageExecution] Damage=%d FinalDamage=0 Target=%s"), DamageData.DamageValue, *GetNameSafe(TargetCharacter));
		return 0;
	}

	const int32 PreviousHP = FMath::RoundToInt(TargetCharacter->GetCurrentHP());
	const int32 NewHP = FMath::Max(0, PreviousHP - FinalDamage);
	TargetCharacter->SetCurrentHP(NewHP);

	const int32 AppliedDamage = PreviousHP - NewHP;
	UE_LOG(LogTemp, Log, TEXT("[DamageExecution] Damage=%d FinalDamage=%d AppliedDamage=%d Target=%s NewHP=%d"), DamageData.DamageValue, FinalDamage, AppliedDamage, *GetNameSafe(TargetCharacter), NewHP);

	return AppliedDamage;
}

void UDamageExecution::BuildHitResponseExecutionEntries(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter, int32 AppliedDamage, FName HitReactionAnimKey, TArray<FBattleExecutionEntry>& OutExecutionEntries) const
{
	if (!TargetCharacter)
		return;

	if (DamageData.bTriggerHitReaction && (AppliedDamage > 0 || !HitReactionAnimKey.IsNone()))
	{
		FBattleExecutionEntry HitReactionEntry;
		HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();

		if (!HitReactionAnimKey.IsNone())
		{
			FHitReactionExecutionData HitReactionData;
			HitReactionData.AnimKey = HitReactionAnimKey;
			HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
		}

		OutExecutionEntries.Add(MoveTemp(HitReactionEntry));
	}

	if (AppliedDamage <= 0)
		return;

	if (Context.ExecutionMode != EBattleExecutionMode::ActualBattle || !DamageData.bTriggerStatusEffectReactions)
		return;

	if (Context.Attacker)
	{
		if (UMuksiStatusEffectComponent* AttackerStatusEffects = Context.Attacker->GetStatusEffectComponent())
			AttackerStatusEffects->AppendHitDealtExecutionEntries(Context, AppliedDamage, OutExecutionEntries);
	}

	if (UMuksiStatusEffectComponent* TargetStatusEffects = TargetCharacter->GetStatusEffectComponent())
		TargetStatusEffects->AppendHitReceivedExecutionEntries(Context, AppliedDamage, OutExecutionEntries);
}

void UDamageExecution::CompleteDamageExecution()
{
	if (IsExecutionFinished())
		return;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UDamageExecution::GetExecutionDataStruct() const
{
	return FDamageExecutionData::StaticStruct();
}
