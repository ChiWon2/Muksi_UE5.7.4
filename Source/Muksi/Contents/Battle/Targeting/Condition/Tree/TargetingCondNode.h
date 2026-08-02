#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/ConditionHandle/CondTree/CondNode.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Base/Cond_Targeting_Base.h"
#include "TargetingCondNode.generated.h"

/** Targeting Condition만 선택할 수 있는 전용 Condition Tree Node. */
USTRUCT(BlueprintType)
struct MUKSI_API FTargetingCondNode
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Condition")
    ECondNodeType Type = ECondNodeType::Leaf;

    UPROPERTY(EditAnywhere, Category="Condition",
        meta=(BaseStruct="/Script/Muksi.Cond_Targeting_Base",
              EditCondition="Type == ECondNodeType::Leaf", EditConditionHides))
    FInstancedStruct Condition;

    UPROPERTY(EditAnywhere, Category="Condition",
        meta=(BaseStruct="/Script/Muksi.TargetingCondNode",
              EditCondition="Type != ECondNodeType::Leaf", EditConditionHides))
    TArray<FInstancedStruct> Children;
};
