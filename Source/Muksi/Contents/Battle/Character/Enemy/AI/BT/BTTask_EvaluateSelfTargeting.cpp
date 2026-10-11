#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_EvaluateSelfTargeting.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_EvaluateSelfTargeting::UBTTask_EvaluateSelfTargeting()
{
    NodeName = TEXT("Evaluate Self Targeting");
}

EBTNodeResult::Type UBTTask_EvaluateSelfTargeting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->EvaluateSelfTargeting() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
