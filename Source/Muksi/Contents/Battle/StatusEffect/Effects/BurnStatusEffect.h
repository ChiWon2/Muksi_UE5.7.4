#pragma once

#include "CoreMinimal.h"
#include "../MuksiStatusEffect.h"
#include "BurnStatusEffect.generated.h"

UCLASS()
class MUKSI_API UBurnStatusEffect : public UMuksiStatusEffect
{
    GENERATED_BODY()

public:
    virtual void OnRoundStart() override;
};