#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateOpponentTargeting.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateOpponentTargeting : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateOpponentTargeting();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
