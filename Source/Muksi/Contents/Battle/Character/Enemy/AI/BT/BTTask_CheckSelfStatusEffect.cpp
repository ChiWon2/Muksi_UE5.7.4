#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTTask_CheckSelfStatusEffect.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTTask_CheckSelfStatusEffect::UBTTask_CheckSelfStatusEffect()
{
    NodeName = TEXT("Check Self Status Effect");
}

EBTNodeResult::Type UBTTask_CheckSelfStatusEffect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return EBTNodeResult::Failed;

    const UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return EBTNodeResult::Failed;

    return AI->CheckSelfStatusEffect(EffectID, MinimumStack) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
