#pragma once

#include "CoreMinimal.h"
#include "ConditionTarget.generated.h"

UENUM(BlueprintType)
enum class EConditionTarget : uint8
{
	Self    UMETA(DisplayName = "Self (Player)"),
	Target  UMETA(DisplayName = "Target"),
	Ally    UMETA(DisplayName = "Ally"),
	Custom  UMETA(DisplayName = "Custom")
};