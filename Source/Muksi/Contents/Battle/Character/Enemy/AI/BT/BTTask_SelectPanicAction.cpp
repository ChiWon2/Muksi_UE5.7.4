#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_SelectPanicAction.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectPanicAction::UBTTask_SelectPanicAction()
{
    NodeName = TEXT("Select Panic Action");
}

EBTNodeResult::Type UBTTask_SelectPanicAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->SelectPanicAction() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
