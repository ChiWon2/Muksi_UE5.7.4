#include "ParalysisStatusEffect.h"

void UParalysisStatusEffect::OnRoundStart()
{
    UE_LOG(LogTemp,Warning,TEXT("[Paralysis] Speed Down"));

    ConsumeDuration();
}
