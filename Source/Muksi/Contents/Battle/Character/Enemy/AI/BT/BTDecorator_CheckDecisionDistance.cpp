#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTDecorator_CheckDecisionDistance.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

UBTDecorator_CheckDecisionDistance::UBTDecorator_CheckDecisionDistance()
{
    NodeName = TEXT("Decision Distance");
}

bool UBTDecorator_CheckDecisionDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Blackboard)
        return false;

    const UMuksiBattleAIComponent* AI = Cast<UMuksiBattleAIComponent>(Blackboard->GetValueAsObject(TEXT("DecisionComponent")));

    if (!AI)
        return false;

    const int32 Distance = AI->GetDecisionDistance();

    if (Distance < 0)
        return false;

    return Condition == EEnemyDistanceCheck::Close ? Distance <= DistanceThreshold : Distance > DistanceThreshold;
}

FString UBTDecorator_CheckDecisionDistance::GetStaticDescription() const
{
    return FString::Printf(TEXT("%s\nDistance %s %d"), *Super::GetStaticDescription(), Condition == EEnemyDistanceCheck::Close ? TEXT("<=") : TEXT(">"), DistanceThreshold);
}
