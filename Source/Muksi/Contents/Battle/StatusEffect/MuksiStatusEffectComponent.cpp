#include "MuksiStatusEffectComponent.h"

#include "MuksiStatusEffect.h"
#include "MuksiStatusEffectSubsystem.h"

UMuksiStatusEffectComponent::UMuksiStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
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

    NewEffect->Initialize(GetOwner(),this,EffectID,StackCount,Duration);

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

void UMuksiStatusEffectComponent::RemoveStatusEffect(FName EffectID)
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

const TArray<TObjectPtr<UMuksiStatusEffect>>& UMuksiStatusEffectComponent::GetActiveEffects() const
{
    return ActiveEffects;
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

#define PROCESS_STATUS_EFFECT_EVENT(FuncName) \
for (UMuksiStatusEffect* Effect : ActiveEffects) \
{ \
    if (Effect) \
    { \
        Effect->FuncName(); \
    } \
} \
RemoveExpiredEffects();


void UMuksiStatusEffectComponent::OnRoundStart()
{
    PROCESS_STATUS_EFFECT_EVENT(OnRoundStart)
}

void UMuksiStatusEffectComponent::OnExchangeStart()
{
    PROCESS_STATUS_EFFECT_EVENT(OnExchangeStart)
}

void UMuksiStatusEffectComponent::OnAttackStart()
{
    PROCESS_STATUS_EFFECT_EVENT(OnAttackStart)
}

void UMuksiStatusEffectComponent::OnAttackEnd()
{
    PROCESS_STATUS_EFFECT_EVENT(OnAttackEnd)
}

void UMuksiStatusEffectComponent::OnExchangeEnd()
{
    PROCESS_STATUS_EFFECT_EVENT(OnExchangeEnd)
}

void UMuksiStatusEffectComponent::OnRoundEnd()
{
    PROCESS_STATUS_EFFECT_EVENT(OnRoundEnd)
}
#undef PROCESS_STATUS_EFFECT_EVENT



