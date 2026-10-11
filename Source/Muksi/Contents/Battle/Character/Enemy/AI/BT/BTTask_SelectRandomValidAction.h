#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"
#include "BTTask_SelectRandomValidAction.generated.h"

UCLASS()
class MUKSI_API UBTTask_SelectRandomValidAction : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SelectRandomValidAction();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
