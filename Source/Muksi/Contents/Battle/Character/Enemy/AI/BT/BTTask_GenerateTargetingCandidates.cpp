#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_GenerateTargetingCandidates.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GenerateTargetingCandidates::UBTTask_GenerateTargetingCandidates()
{
    NodeName = TEXT("Generate Targeting Candidates");
}

EBTNodeResult::Type UBTTask_GenerateTargetingCandidates::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->GenerateTargetingCandidates() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
