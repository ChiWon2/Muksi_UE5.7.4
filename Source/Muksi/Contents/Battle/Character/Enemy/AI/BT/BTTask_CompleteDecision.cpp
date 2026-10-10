#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_CompleteDecision.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_CompleteDecision::UBTTask_CompleteDecision()
{
    NodeName = TEXT("Complete Decision");
}

EBTNodeResult::Type UBTTask_CompleteDecision::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AIComponent = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AIComponent)
        return EBTNodeResult::Failed;

    return AIComponent->CompleteDecision() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
