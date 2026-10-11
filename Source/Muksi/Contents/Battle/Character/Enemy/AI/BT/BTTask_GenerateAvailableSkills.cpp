#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_GenerateAvailableSkills.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GenerateAvailableSkills::UBTTask_GenerateAvailableSkills()
{
    NodeName = TEXT("Generate Available Skills");
}

EBTNodeResult::Type UBTTask_GenerateAvailableSkills::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->GenerateAvailableSkills() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
