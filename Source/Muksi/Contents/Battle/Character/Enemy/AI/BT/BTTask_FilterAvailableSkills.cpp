#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_FilterAvailableSkills.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_FilterAvailableSkills::UBTTask_FilterAvailableSkills()
{
    NodeName = TEXT("Filter Available Skills");
}

EBTNodeResult::Type UBTTask_FilterAvailableSkills::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->FilterAvailableSkills(AllowedCardTypes, AllowedCardIDs) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
