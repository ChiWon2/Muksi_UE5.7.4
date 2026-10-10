#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_AnalyzeOpponent.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_AnalyzeOpponent::UBTTask_AnalyzeOpponent()
{
    NodeName = TEXT("Analyze Opponent");
}

EBTNodeResult::Type UBTTask_AnalyzeOpponent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AIComponent = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AIComponent)
        return EBTNodeResult::Failed;

    return AIComponent->AnalyzeOpponent() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
