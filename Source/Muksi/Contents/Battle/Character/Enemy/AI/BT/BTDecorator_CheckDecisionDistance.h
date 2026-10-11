#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckDecisionDistance.generated.h"

UENUM()
enum class EEnemyDistanceCheck : uint8
{
    Close,
    Far
};

UCLASS()
class MUKSI_API UBTDecorator_CheckDecisionDistance : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_CheckDecisionDistance();

    UPROPERTY(EditAnywhere, Category = "AI|Distance", meta = (ClampMin = "0"))
    int32 DistanceThreshold = 2;

    UPROPERTY(EditAnywhere, Category = "AI|Distance")
    EEnemyDistanceCheck Condition = EEnemyDistanceCheck::Close;

    virtual FString GetStaticDescription() const override;

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
