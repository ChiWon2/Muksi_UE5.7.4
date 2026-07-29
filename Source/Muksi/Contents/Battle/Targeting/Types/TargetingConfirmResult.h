#pragma once

#include "CoreMinimal.h"
#include "TargetingConfirmResult.generated.h"

UENUM(BlueprintType)
enum class ETargetingConfirmResult : uint8
{
	Failed UMETA(DisplayName = "Failed"),
	AdvancedToNextStep UMETA(DisplayName = "Advanced To Next Step"),
	Completed UMETA(DisplayName = "Completed")
};