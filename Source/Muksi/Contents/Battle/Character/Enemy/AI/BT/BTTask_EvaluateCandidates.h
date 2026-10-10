#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateCandidates.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateCandidates : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateCandidates();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
