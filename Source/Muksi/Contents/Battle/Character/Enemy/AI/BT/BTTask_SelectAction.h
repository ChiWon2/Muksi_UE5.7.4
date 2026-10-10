#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectAction.generated.h"

UCLASS()
class MUKSI_API UBTTask_SelectAction : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SelectAction();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
