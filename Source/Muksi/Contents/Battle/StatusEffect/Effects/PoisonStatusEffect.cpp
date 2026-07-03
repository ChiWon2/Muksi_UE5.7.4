#include "PoisonStatusEffect.h"

void UPoisonStatusEffect::OnRoundStart()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[Poison] Triggered | Stack : %d"),
        GetCurrentStack());

    ConsumeDuration();
}