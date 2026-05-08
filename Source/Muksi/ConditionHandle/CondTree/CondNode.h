#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include"../Condition/Cond_Base.h"
#include "CondNode.generated.h"

UENUM(BlueprintType)
enum class ECondNodeType : uint8
{
    Leaf UMETA(DisplayName = "Condition"),
    And  UMETA(DisplayName = "AND"),
    Or   UMETA(DisplayName = "OR"),
    Not  UMETA(DisplayName = "NOT")
};

USTRUCT(BlueprintType)
struct FCondNode : public FCond_Base
{
    GENERATED_BODY()

    // 노드 타입
    UPROPERTY(EditAnywhere)
    ECondNodeType Type = ECondNodeType::Leaf;

    // Leaf일 때만 사용
    UPROPERTY(EditAnywhere,meta = (BaseStruct = "/Script/Muksi.Cond_Base", EditCondition = "Type == ECondNodeType::Leaf", EditConditionHides))
    FInstancedStruct Condition;

    UPROPERTY(EditAnywhere,meta = (BaseStruct = "/Script/Muksi.CondNode", EditCondition = "Type != ECondNodeType::Leaf", EditConditionHides))
    TArray<FInstancedStruct> Children;
};