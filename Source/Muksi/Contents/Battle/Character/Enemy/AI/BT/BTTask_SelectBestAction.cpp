#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_SelectBestAction.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectBestAction::UBTTask_SelectBestAction()
{
    NodeName = TEXT("Select Best Action");
}

EBTNodeResult::Type UBTTask_SelectBestAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->SelectBestAction() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
