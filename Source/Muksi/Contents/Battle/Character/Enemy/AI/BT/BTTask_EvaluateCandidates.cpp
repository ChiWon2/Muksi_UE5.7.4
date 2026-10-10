#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateCandidates.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_EvaluateCandidates::UBTTask_EvaluateCandidates()
{
    NodeName = TEXT("Evaluate Candidates");
}

EBTNodeResult::Type UBTTask_EvaluateCandidates::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AIComponent = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AIComponent)
        return EBTNodeResult::Failed;

    return AIComponent->EvaluateCandidates() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
