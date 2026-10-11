#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateSelfTargeting.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateSelfTargeting : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateSelfTargeting();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
