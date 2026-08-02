#pragma once

#include "CoreMinimal.h"
#include "Muksi/ConditionHandle/Condition/Cond_Base.h"
#include "Cond_Targeting_Base.generated.h"

/** Targeting Step 전용 Condition 분류 기준. */
USTRUCT(BlueprintType, meta=(Hidden))
struct MUKSI_API FCond_Targeting_Base : public FCond_Base
{
    GENERATED_BODY()
};
