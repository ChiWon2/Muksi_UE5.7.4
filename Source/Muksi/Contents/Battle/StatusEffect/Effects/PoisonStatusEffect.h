#pragma once

#include "CoreMinimal.h"
#include "../MuksiStatusEffect.h"
#include "PoisonStatusEffect.generated.h"

UCLASS()
class MUKSI_API UPoisonStatusEffect : public UMuksiStatusEffect
{
    GENERATED_BODY()

public:
    virtual void OnRoundStart() override;
};