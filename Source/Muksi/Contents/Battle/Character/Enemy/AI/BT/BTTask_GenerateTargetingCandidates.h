#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"
#include "BTTask_GenerateTargetingCandidates.generated.h"

UCLASS()
class MUKSI_API UBTTask_GenerateTargetingCandidates : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GenerateTargetingCandidates();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
