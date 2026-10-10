#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnalyzeOpponent.generated.h"

UCLASS()
class MUKSI_API UBTTask_AnalyzeOpponent : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_AnalyzeOpponent();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
