#include "Muksi/Contents/Battle/StatusEffect/Effects/InsightStatusEffect.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/BattleCamera/PlayBattleCameraExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/BattleCamera/PlayBattleCameraExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StopMontage/StopMontageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StopMontage/StopMontageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/Rotate/RotateExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Rotate/RotateExecutionData.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

UInsightStatusEffect::UInsightStatusEffect()
{
	BattleActionEditPriority = 1000;

	FBattleExecutionEntry CameraEntry;
	CameraEntry.ExecutionClass = UPlayBattleCameraExecution::StaticClass();
	CameraEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FPlayBattleCameraExecutionData CameraData;
	CameraData.CameraKey = TEXT("Insight_Counter");

	CameraEntry.ExecutionData.InitializeAs<FPlayBattleCameraExecutionData>(CameraData);
	ModifyExecutionEntries.Add(MoveTemp(CameraEntry));

	FBattleExecutionEntry OwnerRotateEntry;
	OwnerRotateEntry.ExecutionClass = URotateExecution::StaticClass();
	OwnerRotateEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FRotateExecutionData OwnerRotateData;
	OwnerRotateData.TargetMode = ERotateExecutionTargetMode::Opponent;

	OwnerRotateEntry.ExecutionData.InitializeAs<FRotateExecutionData>(OwnerRotateData);
	ModifyExecutionEntries.Add(MoveTemp(OwnerRotateEntry));

	FBattleExecutionEntry OpponentRotateEntry;
	OpponentRotateEntry.ExecutionClass = URotateExecution::StaticClass();
	OpponentRotateEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FRotateExecutionData OpponentRotateData;
	OpponentRotateData.TargetMode = ERotateExecutionTargetMode::Opponent;

	OpponentRotateEntry.ExecutionData.InitializeAs<FRotateExecutionData>(OpponentRotateData);
	ModifyExecutionEntries.Add(MoveTemp(OpponentRotateEntry));

	FBattleExecutionEntry OpponentAttackEntry;
	OpponentAttackEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
	OpponentAttackEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FPlayMontageExecutionData OpponentAttackData;
	OpponentAttackData.AnimKey = TEXT("Attack_1");

	OpponentAttackEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(OpponentAttackData);
	ModifyExecutionEntries.Add(MoveTemp(OpponentAttackEntry));

	FBattleExecutionNotify OpponentPreMainEffectNotify;
	OpponentPreMainEffectNotify.NotifyKey = TEXT("PreMainEffect");
	OpponentPreMainEffectNotify.SourceAnimKey = TEXT("Attack_1");

	FBattleExecutionEntry CounterAttackEntry;
	CounterAttackEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
	CounterAttackEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FPlayMontageExecutionData CounterAttackData;
	CounterAttackData.AnimKey = TEXT("Attack_2");

	CounterAttackEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(CounterAttackData);
	OpponentPreMainEffectNotify.ExecutionEntries.Add(MoveTemp(CounterAttackEntry));
	ModifyExecutionNotifies.Add(MoveTemp(OpponentPreMainEffectNotify));

	FBattleExecutionNotify CounterMainEffectNotify;
	CounterMainEffectNotify.NotifyKey = TEXT("MainEffect");
	CounterMainEffectNotify.SourceAnimKey = TEXT("Attack_2");

	FBattleExecutionEntry StopMontageEntry;
	StopMontageEntry.ExecutionClass = UStopMontageExecution::StaticClass();
	StopMontageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStopMontageExecutionData StopMontageData;
	StopMontageData.BlendOutTime = 0.0f;

	StopMontageEntry.ExecutionData.InitializeAs<FStopMontageExecutionData>(StopMontageData);
	CounterMainEffectNotify.ExecutionEntries.Add(MoveTemp(StopMontageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.AnimKey = TEXT("HitReaction_Fall");

	HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
	CounterMainEffectNotify.ExecutionEntries.Add(MoveTemp(HitReactionEntry));
	ModifyExecutionNotifies.Add(MoveTemp(CounterMainEffectNotify));

	FBattleExecutionNotify CounterDamageEffectNotify;
	CounterDamageEffectNotify.NotifyKey = TEXT("DamageEffect");
	CounterDamageEffectNotify.SourceAnimKey = TEXT("Attack_2");

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	DamageData.DefensePolicy = EDamageDefensePolicy::IgnoreDefense;
	DamageData.HitFXDataAssetKey = TEXT("HitReaction_Basic");
	DamageData.bTriggerHitReaction = false;

	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	CounterDamageEffectNotify.ExecutionEntries.Add(MoveTemp(DamageEntry));
	ModifyExecutionNotifies.Add(MoveTemp(CounterDamageEffectNotify));

	FBattleExecutionNotify CounterSubEffectNotify;
	CounterSubEffectNotify.NotifyKey = TEXT("SubEffect");
	CounterSubEffectNotify.SourceAnimKey = TEXT("Attack_2");

	FBattleExecutionEntry RestorePresentationEntry;
	RestorePresentationEntry.ExecutionClass = URestorePresentationExecution::StaticClass();
	RestorePresentationEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FRestorePresentationExecutionData RestorePresentationData;
	RestorePresentationData.MoveDuration = 0.08f;

	RestorePresentationEntry.ExecutionData.InitializeAs<FRestorePresentationExecutionData>(RestorePresentationData);
	CounterSubEffectNotify.ExecutionEntries.Add(MoveTemp(RestorePresentationEntry));
	ModifyExecutionNotifies.Add(MoveTemp(CounterSubEffectNotify));
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
	static_cast<void>(CurrentAction);

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
		if (StepResult.ContainsTarget(OwnerCharacter))
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

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;
	TArray<FBattleExecutionNotify> ExecutionNotifies = ModifyExecutionNotifies;

	if (ExecutionEntries.IsValidIndex(0))
	{
		ExecutionEntries[0].ExecutionSourceOverride = OwnerCharacter;
		ExecutionEntries[0].ExecutionTargetOverride = OpponentCharacter;
	}

	if (ExecutionEntries.IsValidIndex(1))
	{
		ExecutionEntries[1].ExecutionSourceOverride = OwnerCharacter;
		ExecutionEntries[1].ExecutionTargetOverride = OpponentCharacter;
	}

	if (ExecutionEntries.IsValidIndex(2))
	{
		ExecutionEntries[2].ExecutionSourceOverride = OpponentCharacter;
		ExecutionEntries[2].ExecutionTargetOverride = OwnerCharacter;
	}

	if (ExecutionEntries.IsValidIndex(3))
	{
		ExecutionEntries[3].ExecutionSourceOverride = OpponentCharacter;
		ExecutionEntries[3].ExecutionTargetOverride = OwnerCharacter;
	}

	for (FBattleExecutionNotify& Notify : ExecutionNotifies)
	{
		Notify.NotifySourceOverride = Notify.SourceAnimKey == TEXT("Attack_1") ? OpponentCharacter : OwnerCharacter;

		for (FBattleExecutionEntry& Entry : Notify.ExecutionEntries)
		{
			Entry.ExecutionSourceOverride = Entry.ExecutionClass == UStopMontageExecution::StaticClass() ? OpponentCharacter : OwnerCharacter;
			Entry.ExecutionTargetOverride = OpponentCharacter;

			FDamageExecutionData* DamageData = Entry.ExecutionData.GetMutablePtr<FDamageExecutionData>();
			if (DamageData)
				DamageData->DamageValue = GetCurrentStack();
		}
	}

	OpponentAction.ExecutionEntries = MoveTemp(ExecutionEntries);
	OpponentAction.ExecutionNotifies = MoveTemp(ExecutionNotifies);
	OpponentAction.bStatusEffectEditLocked = true;
}

void UInsightStatusEffect::OnBattleExchangeCompleted(int32 ExchangeIndex)
{
	static_cast<void>(ExchangeIndex);
	ConsumeDuration(GetRemainingDuration());
}
