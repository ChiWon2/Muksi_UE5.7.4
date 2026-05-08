#pragma once

#include "CoreMinimal.h"
#include "ConditionCompareOp.generated.h"

UENUM(BlueprintType)
enum class EConditionCompareOp : uint8
{
	Equal UMETA(DisplayName = "Equal (==)"),
	NotEqual UMETA(DisplayName = "Not Equal (!=)"),

	Greater UMETA(DisplayName = "Greater (>)"),
	Less UMETA(DisplayName = "Less (<)"),

	GreaterOrEqual UMETA(DisplayName = "Greater Or Equal (>=)"),
	LessOrEqual UMETA(DisplayName = "Less Or Equal (<=)")
};