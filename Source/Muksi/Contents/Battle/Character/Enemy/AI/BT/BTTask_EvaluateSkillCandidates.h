#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"
#include "BTTask_EvaluateSkillCandidates.generated.h"

UCLASS()
class MUKSI_API UBTTask_EvaluateSkillCandidates : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_EvaluateSkillCandidates();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
