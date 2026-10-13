#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTComposite_WeightedRandomSelector.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/BT/BTDecorator_SelectionWeight.h"

UBTComposite_WeightedRandomSelector::UBTComposite_WeightedRandomSelector()
{
    NodeName = TEXT("Weighted Random Selector");
}

uint16 UBTComposite_WeightedRandomSelector::GetInstanceMemorySize() const
{
    return sizeof(FWeightedRandomSelectorMemory);
}

void UBTComposite_WeightedRandomSelector::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
    Super::InitializeMemory(OwnerComp, NodeMemory, InitType);

    FWeightedRandomSelectorMemory* Memory = reinterpret_cast<FWeightedRandomSelectorMemory*>(NodeMemory);
    Memory->TriedChildren = 0;
}

bool UBTComposite_WeightedRandomSelector::CanAbortLowerPriority() const
{
    return false;
}

int32 UBTComposite_WeightedRandomSelector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
    if (PrevChild != BTSpecialChild::NotInitialized && LastResult == EBTNodeResult::Succeeded)
        return BTSpecialChild::ReturnToParent;

    FWeightedRandomSelectorMemory* Memory = GetNodeMemory<FWeightedRandomSelectorMemory>(SearchData);

    if (PrevChild == BTSpecialChild::NotInitialized)
        Memory->TriedChildren = 0;
    else if (PrevChild >= 0 && PrevChild < 64)
        Memory->TriedChildren |= (uint64(1) << PrevChild);

    // Runtime memory uses a compact mask because a decision should have few branches.
    const int32 ChildCount = FMath::Min(GetChildrenNum(), 64);
    float TotalWeight = 0.f;
    TArray<float, TInlineAllocator<8>> Weights;
    Weights.SetNumZeroed(ChildCount);

    for (int32 Index = 0; Index < ChildCount; ++Index)
    {
        if ((Memory->TriedChildren & (uint64(1) << Index)) != 0)
            continue;

        if (!DoDecoratorsAllowExecution(SearchData.OwnerComp, SearchData.OwnerComp.GetActiveInstanceIdx(), Index))
            continue;

        float Weight = 1.f;

        for (const UBTDecorator* Decorator : Children[Index].Decorators)
        {
            const UBTDecorator_SelectionWeight* WeightDecorator = Cast<UBTDecorator_SelectionWeight>(Decorator);

            if (WeightDecorator)
                Weight = WeightDecorator->Weight;
        }

        Weights[Index] = FMath::Max(0.f, Weight);
        TotalWeight += Weights[Index];
    }

    if (TotalWeight <= 0.f)
        return BTSpecialChild::ReturnToParent;

    float Roll = FMath::FRand() * TotalWeight;

    for (int32 Index = 0; Index < ChildCount; ++Index)
    {
        if (Weights[Index] <= 0.f)
            continue;

        Roll -= Weights[Index];

        if (Roll <= 0.f)
            return Index;
    }

    for (int32 Index = ChildCount - 1; Index >= 0; --Index)
    {
        if (Weights[Index] > 0.f)
            return Index;
    }

    return BTSpecialChild::ReturnToParent;
}
