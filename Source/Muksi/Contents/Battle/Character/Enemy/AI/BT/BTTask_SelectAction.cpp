#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_SelectAction.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_SelectAction::UBTTask_SelectAction()
{
    NodeName = TEXT("Select Action");
}

EBTNodeResult::Type UBTTask_SelectAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AIComponent = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AIComponent)
        return EBTNodeResult::Failed;

    return AIComponent->SelectAction() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
