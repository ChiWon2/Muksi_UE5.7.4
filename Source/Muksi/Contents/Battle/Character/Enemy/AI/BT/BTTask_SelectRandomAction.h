#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectRandomAction.generated.h"

UCLASS()
class MUKSI_API UBTTask_SelectRandomAction : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SelectRandomAction();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
