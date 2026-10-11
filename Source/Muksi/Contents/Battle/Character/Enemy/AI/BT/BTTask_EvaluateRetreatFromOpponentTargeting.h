#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateRetreatFromOpponentTargeting.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateRetreatFromOpponentTargeting : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateRetreatFromOpponentTargeting();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
