#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_GenerateCandidates.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_GenerateCandidates::UBTTask_GenerateCandidates()
{
    NodeName = TEXT("Generate Candidates");
}

EBTNodeResult::Type UBTTask_GenerateCandidates::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AIComponent = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AIComponent)
        return EBTNodeResult::Failed;

    return AIComponent->GenerateCandidates() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
