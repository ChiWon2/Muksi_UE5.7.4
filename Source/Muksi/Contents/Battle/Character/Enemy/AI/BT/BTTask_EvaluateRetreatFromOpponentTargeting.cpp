#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateRetreatFromOpponentTargeting.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_EvaluateRetreatFromOpponentTargeting::UBTTask_EvaluateRetreatFromOpponentTargeting()
{
    NodeName = TEXT("Evaluate RetreatFromOpponent Targeting");
}

EBTNodeResult::Type UBTTask_EvaluateRetreatFromOpponentTargeting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->EvaluateRetreatFromOpponentTargeting() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
