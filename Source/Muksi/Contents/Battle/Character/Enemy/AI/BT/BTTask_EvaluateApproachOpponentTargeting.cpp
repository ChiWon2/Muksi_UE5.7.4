#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateApproachOpponentTargeting.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_EvaluateApproachOpponentTargeting::UBTTask_EvaluateApproachOpponentTargeting()
{
    NodeName = TEXT("Evaluate ApproachOpponent Targeting");
}

EBTNodeResult::Type UBTTask_EvaluateApproachOpponentTargeting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->EvaluateApproachOpponentTargeting() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
