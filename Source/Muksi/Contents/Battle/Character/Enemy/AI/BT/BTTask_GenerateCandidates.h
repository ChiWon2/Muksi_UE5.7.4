#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GenerateCandidates.generated.h"

UCLASS()
class MUKSI_API UBTTask_GenerateCandidates : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GenerateCandidates();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
