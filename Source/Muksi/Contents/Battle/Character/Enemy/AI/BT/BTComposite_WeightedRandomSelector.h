#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_WeightedRandomSelector.generated.h"

// First 64 children are supported. The BT editor should keep each selector small.
struct FWeightedRandomSelectorMemory : public FBTCompositeMemory
{
    uint64 TriedChildren = 0;
};

UCLASS()
class MUKSI_API UBTComposite_WeightedRandomSelector : public UBTCompositeNode
{
    GENERATED_BODY()

public:
    UBTComposite_WeightedRandomSelector();

    virtual uint16 GetInstanceMemorySize() const override;
    virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;

protected:
    virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
    virtual bool CanAbortLowerPriority() const override;
};
