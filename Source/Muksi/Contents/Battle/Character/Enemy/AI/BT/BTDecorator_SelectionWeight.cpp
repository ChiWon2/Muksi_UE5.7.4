#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTDecorator_SelectionWeight.h"

UBTDecorator_SelectionWeight::UBTDecorator_SelectionWeight()
{
    NodeName = TEXT("Selection Weight");
}

bool UBTDecorator_SelectionWeight::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    return Weight > 0.f;
}

FString UBTDecorator_SelectionWeight::GetStaticDescription() const
{
    return FString::Printf(TEXT("%s\nWeight: %.2f"), *Super::GetStaticDescription(), Weight);
}
