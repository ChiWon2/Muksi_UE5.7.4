#pragma once

#include "CoreMinimal.h"
#include "BattleGridIndicatorType.generated.h"

UENUM(BlueprintType)
enum class EBattleGridIndicatorType : uint8
{
	None,
	Hover,
	Target,
	Movement,
	Attack,
	Exchange
};
