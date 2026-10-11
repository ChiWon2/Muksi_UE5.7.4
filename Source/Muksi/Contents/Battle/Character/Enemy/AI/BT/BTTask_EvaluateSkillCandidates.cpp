#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateSkillCandidates.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_EvaluateSkillCandidates::UBTTask_EvaluateSkillCandidates()
{
    NodeName = TEXT("Evaluate Skill Candidates");
}

EBTNodeResult::Type UBTTask_EvaluateSkillCandidates::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->EvaluateSkillCandidates() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
