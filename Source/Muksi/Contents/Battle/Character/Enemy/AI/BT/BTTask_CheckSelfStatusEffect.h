#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckSelfStatusEffect.generated.h"

UCLASS()
class MUKSI_API UBTTask_CheckSelfStatusEffect : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_CheckSelfStatusEffect();

    UPROPERTY(EditAnywhere, Category = "AI|Status Effect")
    FName EffectID = NAME_None;

    UPROPERTY(EditAnywhere, Category = "AI|Status Effect", meta = (ClampMin = "1"))
    int32 MinimumStack = 1;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
