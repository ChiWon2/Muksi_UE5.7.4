#include "BurnStatusEffect.h"

void UBurnStatusEffect::OnRoundStart()
{
    UE_LOG(LogTemp,Warning,TEXT("[Burn] Triggered | Stack : %d"),GetCurrentStack());

    ConsumeDuration();
}