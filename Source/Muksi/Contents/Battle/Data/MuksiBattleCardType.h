#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardType.generated.h"

UENUM(BlueprintType)
enum class EMuksiBattleCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Rush		UMETA(DisplayName = "Rush"),
	RangeAttack	UMETA(DisplayName = "Range Attack"),
	Defense		UMETA(DisplayName = "Defense"),
	Heal		UMETA(DisplayName = "Heal"),
	Move		UMETA(DisplayName = "Move"),
};