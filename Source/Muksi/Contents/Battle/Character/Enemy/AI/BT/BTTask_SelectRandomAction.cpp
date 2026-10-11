#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_SelectRandomAction.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_SelectRandomAction::UBTTask_SelectRandomAction()
{
    NodeName = TEXT("Select Random Action");
}

EBTNodeResult::Type UBTTask_SelectRandomAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->SelectRandomAction() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
