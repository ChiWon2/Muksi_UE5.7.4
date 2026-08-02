#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingCondNode.h"

class FTargetingConditionTreeEvaluator
{
public:
    static bool Evaluate(UObject* WorldContextObject, const FTargetingCondNode& Node);
};
