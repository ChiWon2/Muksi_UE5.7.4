#include "MuksiStatusEffectComponent.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "MuksiStatusEffect.h"
#include "MuksiStatusEffectSubsystem.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"

UMuksiStatusEffectComponent::UMuksiStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiStatusEffectComponent::Initialize(ABattleManager* InBattleManager)
{
    BattleManager = InBattleManager;
}

void UMuksiStatusEffectComponent::CopyRuntimeStateFrom(const UMuksiStatusEffectComponent& SourceComponent)
{
    FinishExecution();
    ActiveEffects.Reset();
    for (UMuksiStatusEffect* SourceEffect : SourceComponent.ActiveEffects)
    {
        if (!IsValid(SourceEffect)) continue;
        UMuksiStatusEffect* NewEffect = NewObject<UMuksiStatusEffect>(this, SourceEffect->GetClass());
        if (!IsValid(NewEffect)) continue;
        NewEffect->CopyRuntimeStateFrom(*SourceEffect, GetOwner());
        ActiveEffects.Add(NewEffect);
    }
}

void UMuksiStatusEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    FinishExecution();

    BattleManager = nullptr;

    Super::EndPlay(EndPlayReason);
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::AddStatusEffect(FName EffectID,int32 StackCount,int32 Duration)
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    StackCount = FMath::Max(1, StackCount);
    Duration = FMath::Max(1, Duration);

    UMuksiStatusEffectSubsystem* StatusEffectSubsystem = UMuksiStatusEffectSubsystem::Get(this);

    TSubclassOf<UMuksiStatusEffect> EffectClass = StatusEffectSubsystem->FindEffectClass(EffectID);

    if (!EffectClass)
    {
        UE_LOG(LogTemp, Error,TEXT("[StatusEffectComponent] Cannot find EffectClass. EffectID: %s"),*EffectID.ToString());
        return nullptr;
    }

    if (UMuksiStatusEffect* ExistingEffect = FindEffectByID(EffectID))
    {
        ExistingEffect->OnReapplied(StackCount, Duration);

        OnStatusEffectsChanged.Broadcast();

        return ExistingEffect;
    }

    UMuksiStatusEffect* NewEffect = NewObject<UMuksiStatusEffect>(this,EffectClass);

    NewEffect->Initialize(GetOwner(),EffectID,StackCount,Duration);

    ActiveEffects.Add(NewEffect);

    NewEffect->OnApplied();

    OnStatusEffectsChanged.Broadcast();

    return NewEffect;
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::SubtractStatusEffect(FName EffectID,int32 StackCount,int32 Duration)
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

void UMuksiStatusEffectComponent::AppendBattleActionStartExecutions(const FBattleAction& BattleAction, TArray<FBattleExecutionEntry>& OutExecutions) const
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect)) Effect->BuildBattleActionStartExecutions(BattleAction, OutExecutions);
	}
}

void UMuksiStatusEffectComponent::AppendHitDealtExecutions(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutions) const
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect)) Effect->BuildHitDealtExecutions(Context, Damage, OutExecutions);
	}
}

void UMuksiStatusEffectComponent::AppendHitReceivedExecutions(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutions) const
{
	const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
	for (UMuksiStatusEffect* Effect : EffectsSnapshot)
	{
		if (IsValid(Effect)) Effect->BuildHitReceivedExecutions(Context, Damage, OutExecutions);
	}
}

void UMuksiStatusEffectComponent::RemoveExpiredEffects()
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

    if (bRemovedAny)
    {
        OnStatusEffectsChanged.Broadcast();
    }
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

		TArray<FBattleExecutionEntry> PhaseExecutions;
		Effect->BuildPhaseExecutions(ExecutingOldPhase, ExecutingNewPhase, PhaseExecutions);
		if (PhaseExecutions.IsEmpty())
		{
			continue;
		}

		ExecutePhaseExecutions(PhaseExecutions);
		return;
    }

    RemoveExpiredEffects();
    FinishExecution();
}

void UMuksiStatusEffectComponent::ExecutePhaseExecutions(const TArray<FBattleExecutionEntry>& ExecutionEntries)
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
	Context.ExecutionMode = EBattleExecutionMode::Sequence;
	Context.Attacker = OwnerCharacter;
	Context.ExecutionTarget = OwnerCharacter;
	Context.BattleGridManager = BattleManager ? BattleManager->GetBattleGridManager() : nullptr;

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UMuksiStatusEffectComponent::HandlePhaseExecutionRunnerFinished);
	PhaseExecutionRunner->Run(ExecutionEntries, Context, FBattleExecutionEntryStarted(), FBattleExecutionEntryFinished(), OnFinished);
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


