#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "BTTask_FilterAvailableSkills.generated.h"

UCLASS()
class MUKSI_API UBTTask_FilterAvailableSkills : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FilterAvailableSkills();

    UPROPERTY(EditAnywhere, Category = "AI|Filter")
    TArray<FBattleCardTypeInfoData> AllowedCardTypes;

    UPROPERTY(EditAnywhere, Category = "AI|Filter")
    TArray<FName> AllowedCardIDs;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
