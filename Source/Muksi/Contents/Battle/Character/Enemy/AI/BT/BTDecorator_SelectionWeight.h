#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SelectionWeight.generated.h"

UCLASS()
class MUKSI_API UBTDecorator_SelectionWeight : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_SelectionWeight();

    UPROPERTY(EditAnywhere, Category = "AI|Probability", meta = (ClampMin = "0.0"))
    float Weight = 1.f;

    virtual FString GetStaticDescription() const override;

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
