#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CompleteDecision.generated.h"

UCLASS()
class MUKSI_API UBTTask_CompleteDecision : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_CompleteDecision();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
