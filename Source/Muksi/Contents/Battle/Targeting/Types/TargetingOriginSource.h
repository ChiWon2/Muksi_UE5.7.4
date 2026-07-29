#pragma once

#include "CoreMinimal.h"
#include "TargetingOriginSource.generated.h"

UENUM(BlueprintType)
enum class ETargetingOriginSource : uint8
{
	SourceCharacter UMETA(DisplayName = "Source Character"),
	PreviousStep UMETA(DisplayName = "Previous Step"),
	SpecificStep UMETA(DisplayName = "Specific Step")
};