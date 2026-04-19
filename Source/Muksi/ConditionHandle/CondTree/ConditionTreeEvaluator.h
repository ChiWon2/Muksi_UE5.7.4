#pragma once

#include "CoreMinimal.h"
#include "CondNode.h"

class FConditionTreeEvaluator
{
public:
    static bool Evaluate(UObject* WorldContextObject, const FCondNode& Node);
};