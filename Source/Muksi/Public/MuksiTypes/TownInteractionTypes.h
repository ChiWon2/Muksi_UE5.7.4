#pragma once

#include "CoreMinimal.h"
#include "TownInteractionTypes.generated.h"

UENUM(BlueprintType)
enum class ETownInteractionType : uint8
{
	None			UMETA(DisplayName = "None"),
	Inn				UMETA(DisplayName = "Inn"),
	Shop			UMETA(DisplayName = "Shop"),
	TrainingGrounds	UMETA(DisplayName = "TrainingGrounds"),
	Forge			UMETA(DisplayName = "Forge"),
	Shrine			UMETA(DisplayName = "Shrine"),
	TangClan		UMETA(DisplayName = "TangClan"),
	Custom			UMETA(DisplayName = "Custom")
};