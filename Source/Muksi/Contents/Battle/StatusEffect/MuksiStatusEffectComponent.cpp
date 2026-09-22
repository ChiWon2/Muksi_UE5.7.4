#include "MuksiStatusEffectComponent.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "MuksiStatusEffect.h"
#include "MuksiIncomingDamageModifierStatusEffect.h"
#include "MuksiStatusEffectRegistry.h"
#include "MuksiStatusEffectIDs.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

UMuksiStatusEffectComponent::UMuksiStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiStatusEffectComponent::Initialize(ABattleManager* InBattleManager)
{
	if (IsValid(BattleManager) && IsValid(BattleManager->GetBattleSequenceManager()))
	{
		BattleManager->GetBattleSequenceManager()->BattleActionStartedDelegate.RemoveAll(this);
		BattleManager->GetBattleSequenceManager()->BattleActionCompletedDelegate.RemoveAll(this);
		BattleManager->GetBattleSequenceManager()->BattleExchangeCompletedDelegate.RemoveAll(this);
	}

	BattleManager = InBattleManager;

	if (IsValid(BattleManager) && IsValid(BattleManager->GetBattleSequenceManager()))
	{
		BattleManager->GetBattleSequenceManager()->BattleActionStartedDelegate.AddUObject(this, &UMuksiStatusEffectComponent::HandleBattleActionStarted);
		BattleManager->GetBattleSequenceManager()->BattleActionCompletedDelegate.AddUObject(this, &UMuksiStatusEffectComponent::HandleBattleActionCompleted);
		BattleManager->GetBattleSequenceManager()->BattleExchangeCompletedDelegate.AddUObject(this, &UMuksiStatusEffectComponent::HandleBattleExchangeCompleted);
	}
}

void UMuksiStatusEffectComponent::ResetRuntimeState()
{
	FinishExecution();
	const bool bHadActiveEffects = !ActiveEffects.IsEmpty();
	ActiveEffects.Reset();
	if (bHadActiveEffects)
		OnStatusEffectsChanged.Broadcast();
}

void UMuksiStatusEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FinishExecution();

	if (IsValid(BattleManager) && IsValid(BattleManager->GetBattleSequenceManager()))
	{
		BattleManager->GetBattleSequenceManager()->BattleActionStartedDelegate.RemoveAll(this);
		BattleManager->GetBattleSequenceManager()->BattleActionCompletedDelegate.RemoveAll(this);
		BattleManager->GetBattleSequenceManager()->BattleExchangeCompletedDelegate.RemoveAll(this);
	}

	BattleManager = nullptr;

	Super::EndPlay(EndPlayReason);
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::AddStatusEffect(FName EffectID, int32 StackCount, int32 Duration)
{
    if (EffectID.IsNone())
        return nullptr;

    StackCount = FMath::Max(1, StackCount);
    Duration = FMath::Max(1, Duration);

	FName OpposingEffectID = NAME_None;

	if (EffectID == MuksiStatusEffectIDs::ReduceDamage)
		OpposingEffectID = MuksiStatusEffectIDs::MultiplyDamage;
	else if (EffectID == MuksiStatusEffectIDs::MultiplyDamage)
		OpposingEffectID = MuksiStatusEffectIDs::ReduceDamage;

	if (!OpposingEffectID.IsNone())
	{
		if (UMuksiStatusEffect* OpposingEffect = FindEffectByID(OpposingEffectID))
		{
			if (OpposingEffect->GetCurrentStack() > StackCount)
			{
				OpposingEffect->ConsumeStack(StackCount);
				OnStatusEffectsChanged.Broadcast();
				return OpposingEffect;
			}

			StackCount -= OpposingEffect->GetCurrentStack();
			RemoveStatusEffect(OpposingEffect);

			if (StackCount <= 0)
				return nullptr;
		}
	}

    if (UMuksiStatusEffect* ExistingEffect = FindEffectByID(EffectID))
    {
        ExistingEffect->OnReapplied(StackCount, Duration);
        OnStatusEffectsChanged.Broadcast();
        return ExistingEffect;
    }

    if (!IsValid(BattleManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectComponent] BattleManager is unavailable."));
        return nullptr;
    }

    UMuksiStatusEffectRegistry* StatusEffectRegistry = BattleManager->GetStatusEffectRegistry();
    if (!StatusEffectRegistry)
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectComponent] StatusEffectRegistry is unavailable."));
        return nullptr;
    }

    TSubclassOf<UMuksiStatusEffect> EffectClass = StatusEffectRegistry->FindEffectClass(EffectID);
    if (!EffectClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectComponent] Cannot find EffectClass. EffectID: %s"), *EffectID.ToString());
        return nullptr;
    }

    UMuksiStatusEffect* NewEffect = NewObject<UMuksiStatusEffect>(this, EffectClass);
    if (!NewEffect)
        return nullptr;

    NewEffect->Initialize(GetOwner(), EffectID, StackCount, Duration);
    ActiveEffects.Add(NewEffect);
    NewEffect->OnApplied();
    ApplyStatusEffectToCurrentBattleAction(NewEffect);
    OnStatusEffectsChanged.Broadcast();

    return NewEffect;
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::SubtractStatusEffect(FName EffectID, int32 StackCount, int32 Duration)
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    UMuksiStatusEffect* Effect = FindEffectByID(EffectID);
    if (!Effect)
    {
        return nullptr;
    }

    if (StackCount > 0)
    {
        Effect->ConsumeStack(StackCount);
    }

    if (Duration > 0)
    {
        Effect->ConsumeDuration(Duration);
    }

    if (Effect->IsExpired())
    {
        RemoveStatusEffect(Effect);
        return nullptr;
    }

    OnStatusEffectsChanged.Broadcast();

    return Effect;
}

void UMuksiStatusEffectComponent::RemoveStatusEffectByID(FName EffectID)
{
    UMuksiStatusEffect* Effect =  FindEffectByID(EffectID);
    if (Effect == nullptr)
        return;
    RemoveStatusEffect(Effect);
}

void UMuksiStatusEffectComponent::RemoveStatusEffect(UMuksiStatusEffect* Effect)
{
    if (!Effect)
    {
        return;
    }

    Effect->OnRemoved();

    ActiveEffects.Remove(Effect);

    OnStatusEffectsChanged.Broadcast();
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::FindEffectByID(FName EffectID) const
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    for (UMuksiStatusEffect* Effect : ActiveEffects)
    {
        if (Effect && Effect->GetEffectID() == EffectID)
        {
            return Effect;
        }
    }
    return nullptr;
}

int32 UMuksiStatusEffectComponent::GetEffectStackCount(FName EffectID) const
{
    if (EffectID.IsNone())
    {
        return 0;
    }

    for (UMuksiStatusEffect* Effect : ActiveEffects)
    {
        if (Effect && Effect->GetEffectID() == EffectID)
        {
            return Effect->GetCurrentStack();
        }
    }
    return 0;
}

const TArray<TObjectPtr<UMuksiStatusEffect>>& UMuksiStatusEffectComponent::GetActiveEffects() const
{
    return ActiveEffects;
}

UStatusEffectDefinitionDataAsset* UMuksiStatusEffectComponent::FindStatusEffectDefinition(FName EffectID) const
{
    if (!IsValid(BattleManager))
        return nullptr;

    UMuksiStatusEffectRegistry* StatusEffectRegistry = BattleManager->GetStatusEffectRegistry();
    if (!StatusEffectRegistry)
        return nullptr;

    return StatusEffectRegistry->FindDefinition(EffectID);
}

void UMuksiStatusEffectComponent::HandleBattleActionStarted(FBattleAction& CurrentAction, FBattleAction& OpponentAction)
{
	if (CurrentAction.Attacker.Get() != GetOwner() || CurrentAction.bStatusEffectEditLocked)
		return;

	TArray<UMuksiStatusEffect*> EditingEffects;
	EditingEffects.Reserve(ActiveEffects.Num());

	for (UMuksiStatusEffect* Effect : ActiveEffects)
	{
		if (IsValid(Effect))
			EditingEffects.Add(Effect);
	}

	EditingEffects.StableSort([](const UMuksiStatusEffect& A, const UMuksiStatusEffect& B)
	{
		return A.GetBattleActionEditPriority() < B.GetBattleActionEditPriority();
	});

	for (UMuksiStatusEffect* Effect : EditingEffects)
	{
		if (CurrentAction.bStatusEffectEditLocked)
			break;

		Effect->EditBattleActions(CurrentAction, OpponentAction);
	}
}

void UMuksiStatusEffectComponent::HandleBattleActionCompleted(const FBattleAction& CompletedAction)
{
	if (CompletedAction.Attacker.Get() != GetOwner())
		return;

	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;

	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect))
			Effect->OnBattleActionCompleted(CompletedAction);
	}

	RemoveExpiredEffects();
}

void UMuksiStatusEffectComponent::HandleBattleExchangeCompleted(int32 ExchangeIndex)
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;

	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect))
			Effect->OnBattleExchangeCompleted(ExchangeIndex);
	}

	RemoveExpiredEffects();
}

void UMuksiStatusEffectComponent::ApplyStatusEffectToCurrentBattleAction(UMuksiStatusEffect* Effect)
{
	if (!IsValid(Effect) || !IsValid(BattleManager) || !IsValid(BattleManager->GetBattleSequenceManager()))
		return;

	FBattleAction* CurrentAction = nullptr;
	FBattleAction* OpponentAction = nullptr;

	if (!BattleManager->GetBattleSequenceManager()->GetCurrentBattleActionPair(CurrentAction, OpponentAction))
		return;

	if (!CurrentAction || !OpponentAction || CurrentAction->Attacker.Get() != GetOwner() || CurrentAction->bStatusEffectEditLocked)
		return;

	BattleManager->GetBattleSequenceManager()->RefreshBattleActionTargetingResult(*CurrentAction);
	BattleManager->GetBattleSequenceManager()->RefreshBattleActionTargetingResult(*OpponentAction);
	Effect->EditBattleActions(*CurrentAction, *OpponentAction);
}

void UMuksiStatusEffectComponent::AppendHitDealtExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries) const
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect))
			Effect->BuildHitDealtExecutionEntries(Context, Damage, OutExecutionEntries);
	}
}

void UMuksiStatusEffectComponent::AppendHitReceivedExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries) const
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect))
			Effect->BuildHitReceivedExecutionEntries(Context, Damage, OutExecutionEntries);
	}
}

int32 UMuksiStatusEffectComponent::ApplyIncomingDamageModifiers(int32 Damage, FName& OutHitReactionAnimKey)
{
	FIncomingDamageModifierContext IncomingDamageContext;
	IncomingDamageContext.OriginalDamage = FMath::Max(0, Damage);
	IncomingDamageContext.RemainingDamage = IncomingDamageContext.OriginalDamage;

	OutHitReactionAnimKey = NAME_None;

	TArray<UMuksiIncomingDamageModifierStatusEffect*> DamageModifiers;

	for (UMuksiStatusEffect* Effect : ActiveEffects)
	{
		UMuksiIncomingDamageModifierStatusEffect* DamageModifier = Cast<UMuksiIncomingDamageModifierStatusEffect>(Effect);

		if (IsValid(DamageModifier))
			DamageModifiers.Add(DamageModifier);
	}

	DamageModifiers.StableSort([](const UMuksiIncomingDamageModifierStatusEffect& A, const UMuksiIncomingDamageModifierStatusEffect& B)
	{
		return A.GetIncomingDamageModifierPriority() < B.GetIncomingDamageModifierPriority();
	});

	bool bModifierStateChanged = false;

	for (UMuksiIncomingDamageModifierStatusEffect* DamageModifier : DamageModifiers)
	{
		if (IncomingDamageContext.RemainingDamage <= 0)
			break;

		const int32 StackBeforeModifier = DamageModifier->GetCurrentStack();
		const int32 DamageBeforeModifier = IncomingDamageContext.RemainingDamage;

		DamageModifier->ModifyIncomingDamage(IncomingDamageContext);
		IncomingDamageContext.RemainingDamage = FMath::Max(0, IncomingDamageContext.RemainingDamage);

		if (StackBeforeModifier != DamageModifier->GetCurrentStack())
			bModifierStateChanged = true;

		if (OutHitReactionAnimKey.IsNone() && IncomingDamageContext.RemainingDamage < DamageBeforeModifier)
			OutHitReactionAnimKey = DamageModifier->GetHitReactionAnimKey();
	}

	const bool bRemovedExpiredEffect = RemoveExpiredEffects(false);

	if (bModifierStateChanged || bRemovedExpiredEffect)
		OnStatusEffectsChanged.Broadcast();

	return IncomingDamageContext.RemainingDamage;
}

bool UMuksiStatusEffectComponent::RemoveExpiredEffects(bool bNotify)
{
	bool bRemovedAny = false;

	for (int32 Index = ActiveEffects.Num() - 1; Index >= 0; --Index)
	{
		UMuksiStatusEffect* Effect = ActiveEffects[Index];

		if (Effect && Effect->IsExpired())
		{
			Effect->OnRemoved();
			ActiveEffects.RemoveAt(Index);

			bRemovedAny = true;
		}
	}

	if (bRemovedAny && bNotify)
		OnStatusEffectsChanged.Broadcast();

	return bRemovedAny;
}

void UMuksiStatusEffectComponent::ExecuteSequentially(EBattlePhase OldPhase, EBattlePhase NewPhase,FSimpleDelegate CompletionDelegate)
{
    if (bExecuting)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MuksiStatusEffectComponent] Execution is already active."));
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    bExecuting = true;
    ExecutingOldPhase = OldPhase;
    ExecutingNewPhase = NewPhase;
    ExecutionQueue = ActiveEffects;
    ExecutionIndex = 0;
    ExecutionCompletionDelegate = MoveTemp(CompletionDelegate);
    ExecuteNextStatusEffect();
}

void UMuksiStatusEffectComponent::ExecuteNextStatusEffect()
{
    if (!bExecuting)
    {
        return;
    }

    while (ExecutionQueue.IsValidIndex(ExecutionIndex))
    {
        UMuksiStatusEffect* Effect = ExecutionQueue[ExecutionIndex++];
        if (!IsValid(Effect))
        {
            continue;
        }

		TArray<FBattleExecutionEntry> PhaseExecutionEntries;
		Effect->BuildPhaseExecutionEntries(ExecutingOldPhase, ExecutingNewPhase, PhaseExecutionEntries);
		if (PhaseExecutionEntries.IsEmpty())
		{
			continue;
		}

		RunPhaseExecutionEntries(PhaseExecutionEntries);
		return;
    }

    RemoveExpiredEffects();
    FinishExecution();
}

void UMuksiStatusEffectComponent::RunPhaseExecutionEntries(const TArray<FBattleExecutionEntry>& ExecutionEntries)
{
	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());
	if (!IsValid(OwnerCharacter) || ExecutionEntries.IsEmpty())
	{
		ExecuteNextStatusEffect();
		return;
	}

	PhaseExecutionRunner = NewObject<UBattleExecutionRunner>(this);
	if (!PhaseExecutionRunner)
	{
		ExecuteNextStatusEffect();
		return;
	}

	FBattleExecutionContext Context;
	Context.ExecutionMode = EBattleExecutionMode::ActualBattle;
	Context.Attacker = OwnerCharacter;
	Context.ExecutionTarget = OwnerCharacter;
	Context.BattleGridManager = BattleManager ? BattleManager->GetBattleGridManager() : nullptr;

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UMuksiStatusEffectComponent::HandlePhaseExecutionRunnerFinished);
	PhaseExecutionRunner->RunExecutionEntries(ExecutionEntries, Context, FBattleExecutionEntryStarted(), FBattleExecutionEntryFinished(), OnFinished);
}

void UMuksiStatusEffectComponent::HandlePhaseExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bExecuting || FinishedRunner != PhaseExecutionRunner)
	{
		return;
	}

	PhaseExecutionRunner = nullptr;
	ExecuteNextStatusEffect();
}

void UMuksiStatusEffectComponent::FinishExecution()
{
    if (!bExecuting)
    {
        return;
    }

    bExecuting = false;
    ExecutingOldPhase = EBattlePhase::None;
    ExecutingNewPhase = EBattlePhase::None;
	PhaseExecutionRunner = nullptr;
    ExecutionIndex = INDEX_NONE;
    ExecutionQueue.Reset();
    FSimpleDelegate CompletionDelegate = MoveTemp(ExecutionCompletionDelegate);
    ExecutionCompletionDelegate.Unbind();
    CompletionDelegate.ExecuteIfBound();
}


