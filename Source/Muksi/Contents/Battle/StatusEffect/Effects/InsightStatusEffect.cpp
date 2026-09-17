#include "Muksi/Contents/Battle/StatusEffect/Effects/InsightStatusEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/FaceOff/FaceOffExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/FaceOff/FaceOffExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

UInsightStatusEffect::UInsightStatusEffect()
{
	BattleActionEditPriority = 1000;
}

void UInsightStatusEffect::OnApplied()
{
	Super::OnApplied();

	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor.Get());
	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacter->BattleAnimationComponent))
		return;

	OwnerCharacter->BattleAnimationComponent->SetCharacterState(EMuksiBattleCharacterState::Counter);
}

void UInsightStatusEffect::OnRemoved()
{
	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor.Get());
	if (IsValid(OwnerCharacter) && IsValid(OwnerCharacter->BattleAnimationComponent))
		OwnerCharacter->BattleAnimationComponent->SetCharacterState(EMuksiBattleCharacterState::Idle);

	Super::OnRemoved();
}

void UInsightStatusEffect::EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction)
{
	UMuksiBattleCardDataAsset* OpponentExecutionCard = OpponentAction.Card.Get();

	if (!IsValid(OpponentExecutionCard))
		return;

	if (UMuksiBattleCardDataAsset* ActualCard = OpponentExecutionCard->GetActualCard())
		OpponentExecutionCard = ActualCard;

	if (OpponentExecutionCard->CardTypeInfo.CardType != EMuksiBattleCardType::Attack)
		return;

	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor.Get());

	if (!IsValid(OwnerCharacter))
		return;

	bool bTargetsOwner = false;

	for (const FTargetingStepResult& StepResult : OpponentAction.TargetingResult.Steps)
	{
		if (StepResult.Targets.Contains(OwnerCharacter))
		{
			bTargetsOwner = true;
			break;
		}
	}

	if (!bTargetsOwner)
		return;

	ABattleCharacterBase* OpponentCharacter = OpponentAction.Attacker.Get();

	if (!IsValid(OpponentCharacter))
		return;

	if (FHexGridMath::GetHexDistance(OwnerCharacter->GetCharacterCoord(), OpponentCharacter->GetCharacterCoord()) != 1)
		return;

	OpponentAction.ExecutionEntries.Empty();
	OpponentAction.ExecutionNotifies.Empty();

	FBattleExecutionEntry OpponentAttackEntry;
	OpponentAttackEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
	OpponentAttackEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	OpponentAttackEntry.ExecutionSourceOverride = OpponentCharacter;
	OpponentAttackEntry.ExecutionTargetOverride = OwnerCharacter;

	FPlayMontageExecutionData OpponentAttackData;
	OpponentAttackData.AnimKey = TEXT("Attack_1");

	OpponentAttackEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(OpponentAttackData);
	OpponentAction.ExecutionEntries.Add(MoveTemp(OpponentAttackEntry));

	FBattleExecutionNotify OpponentMainEffectNotify;
	OpponentMainEffectNotify.NotifyKey = TEXT("MainEffect");
	OpponentMainEffectNotify.NotifySourceOverride = OpponentCharacter;

	FBattleExecutionEntry FaceOffEntry;
	FaceOffEntry.ExecutionClass = UFaceOffExecution::StaticClass();
	FaceOffEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	FaceOffEntry.ExecutionSourceOverride = OwnerCharacter;
	FaceOffEntry.ExecutionTargetOverride = OpponentCharacter;

	FFaceOffExecutionData FaceOffData;
	FaceOffData.MoveDuration = 0.08f;

	FaceOffEntry.ExecutionData.InitializeAs<FFaceOffExecutionData>(FaceOffData);
	OpponentMainEffectNotify.ExecutionEntries.Add(MoveTemp(FaceOffEntry));

	FBattleExecutionEntry CounterAttackEntry;
	CounterAttackEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
	CounterAttackEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	CounterAttackEntry.ExecutionSourceOverride = OwnerCharacter;
	CounterAttackEntry.ExecutionTargetOverride = OpponentCharacter;

	FPlayMontageExecutionData CounterAttackData;
	CounterAttackData.AnimKey = TEXT("Attack_2");

	CounterAttackEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(CounterAttackData);
	OpponentMainEffectNotify.ExecutionEntries.Add(MoveTemp(CounterAttackEntry));

	OpponentAction.ExecutionNotifies.Add(MoveTemp(OpponentMainEffectNotify));

	FBattleExecutionNotify CounterMainEffectNotify;
	CounterMainEffectNotify.NotifyKey = TEXT("MainEffect");
	CounterMainEffectNotify.NotifySourceOverride = OwnerCharacter;

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	DamageEntry.ExecutionSourceOverride = OwnerCharacter;
	DamageEntry.ExecutionTargetOverride = OpponentCharacter;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	DamageData.DamageValue = GetCurrentStack();
	DamageData.bTriggerHitReaction = false;

	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	CounterMainEffectNotify.ExecutionEntries.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	HitReactionEntry.ExecutionTargetOverride = OpponentCharacter;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.AnimKey = TEXT("HitReaction_Fall");

	HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
	CounterMainEffectNotify.ExecutionEntries.Add(MoveTemp(HitReactionEntry));

	OpponentAction.ExecutionNotifies.Add(MoveTemp(CounterMainEffectNotify));

	FBattleExecutionNotify CounterSubEffectNotify;
	CounterSubEffectNotify.NotifyKey = TEXT("SubEffect");
	CounterSubEffectNotify.NotifySourceOverride = OwnerCharacter;

	FBattleExecutionEntry RestorePresentationEntry;
	RestorePresentationEntry.ExecutionClass = URestorePresentationExecution::StaticClass();
	RestorePresentationEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;
	RestorePresentationEntry.ExecutionSourceOverride = OwnerCharacter;
	RestorePresentationEntry.ExecutionTargetOverride = OpponentCharacter;

	FRestorePresentationExecutionData RestorePresentationData;
	RestorePresentationData.MoveDuration = 0.08f;

	RestorePresentationEntry.ExecutionData.InitializeAs<FRestorePresentationExecutionData>(RestorePresentationData);
	CounterSubEffectNotify.ExecutionEntries.Add(MoveTemp(RestorePresentationEntry));

	OpponentAction.ExecutionNotifies.Add(MoveTemp(CounterSubEffectNotify));

	OpponentAction.bStatusEffectEditLocked = true;
}
void UInsightStatusEffect::OnBattleExchangeCompleted(int32 ExchangeIndex)
{
	static_cast<void>(ExchangeIndex);
	ConsumeDuration(GetRemainingDuration());
}
