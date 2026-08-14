#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleMoveType.generated.h"


UENUM(BlueprintType)
enum class EMuksiBattleMoveType : uint8
{
	None UMETA(DisplayName = "None"),

	Teleport UMETA(DisplayName = "Teleport"),

	Jump UMETA(DisplayName = "Jump"),

	GroundPath UMETA(DisplayName = "Ground Path")
	
};