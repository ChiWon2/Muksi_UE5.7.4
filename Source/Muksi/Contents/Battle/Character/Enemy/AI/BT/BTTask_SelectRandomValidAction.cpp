#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_SelectRandomValidAction.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectRandomValidAction::UBTTask_SelectRandomValidAction()
{
    NodeName = TEXT("Select Random Valid Action");
}

EBTNodeResult::Type UBTTask_SelectRandomValidAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->SelectRandomValidAction() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
