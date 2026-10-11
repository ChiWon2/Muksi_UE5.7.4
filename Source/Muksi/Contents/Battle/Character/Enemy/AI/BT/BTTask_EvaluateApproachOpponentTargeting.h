#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateApproachOpponentTargeting.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateApproachOpponentTargeting : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateApproachOpponentTargeting();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
