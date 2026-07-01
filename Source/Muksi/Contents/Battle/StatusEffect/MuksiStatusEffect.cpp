#include "MuksiStatusEffect.h"

#include "MuksiStatusEffectComponent.h"

void UMuksiStatusEffect::Initialize(AActor* InOwnerActor,UMuksiStatusEffectComponent* InOwnerComponent,FName InEffectID,int32 InStackCount,int32 InDuration)
{
    OwnerActor = InOwnerActor;
    OwnerComponent = InOwnerComponent;

    EffectID = InEffectID;

    CurrentStack = FMath::Max(1, InStackCount);
    RemainingDuration = FMath::Max(1, InDuration);
}

void UMuksiStatusEffect::OnApplied()
{
}

void UMuksiStatusEffect::OnRemoved()
{
}

void UMuksiStatusEffect::OnReapplied(int32 AddedStack,int32 AddedDuration)
{
    // 기본 정책 Stack : Add , Duration : Highest
    AddStack(AddedStack);

    RemainingDuration =FMath::Max(RemainingDuration,AddedDuration);
}

void UMuksiStatusEffect::OnRoundStart()
{
}

void UMuksiStatusEffect::OnExchangeStart()
{
}

void UMuksiStatusEffect::OnAttackStart()
{
}

void UMuksiStatusEffect::OnAttackEnd()
{
}

void UMuksiStatusEffect::OnExchangeEnd()
{
}

void UMuksiStatusEffect::OnRoundEnd()
{
}

bool UMuksiStatusEffect::IsExpired() const
{
    return RemainingDuration <= 0 || CurrentStack <= 0;
}

void UMuksiStatusEffect::AddStack(int32 Amount)
{
    CurrentStack += Amount;
}

void UMuksiStatusEffect::ConsumeStack(int32 Amount)
{
    CurrentStack = FMath::Max(0, CurrentStack - Amount);
}

void UMuksiStatusEffect::SetStack(int32 NewStack)
{
    CurrentStack = FMath::Max(0, NewStack);
}

void UMuksiStatusEffect::AddDuration(int32 Amount)
{
    RemainingDuration += Amount;
}

void UMuksiStatusEffect::ConsumeDuration(int32 Amount)
{
    RemainingDuration = FMath::Max(0, RemainingDuration - Amount);
}

void UMuksiStatusEffect::SetDuration(int32 NewDuration)
{
    RemainingDuration = FMath::Max(0, NewDuration);
}