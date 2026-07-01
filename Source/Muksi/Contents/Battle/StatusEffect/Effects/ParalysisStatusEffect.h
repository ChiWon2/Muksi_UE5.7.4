#pragma once

#include "CoreMinimal.h"
#include "../MuksiStatusEffect.h"
#include "ParalysisStatusEffect.generated.h"

UCLASS()
class MUKSI_API UParalysisStatusEffect : public UMuksiStatusEffect
{
    GENERATED_BODY()

public:
    virtual void OnRoundStart() override;
};