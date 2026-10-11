#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateOpponentTargeting.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_EvaluateOpponentTargeting::UBTTask_EvaluateOpponentTargeting()
{
    NodeName = TEXT("Evaluate Opponent Targeting");
}

EBTNodeResult::Type UBTTask_EvaluateOpponentTargeting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->EvaluateOpponentTargeting() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
