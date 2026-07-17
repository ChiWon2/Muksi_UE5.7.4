#pragma once

#include "CoreMinimal.h"
#include "TargetingGridPreviewMode.generated.h"

UENUM(BlueprintType)
enum class ETargetingGridPreviewMode : uint8
{
	None UMETA(DisplayName = "None"),
	AffectedTiles UMETA(DisplayName = "Affected Tiles"),
	PathTiles UMETA(DisplayName = "Path Tiles"),
	AffectedAndPathTiles UMETA(DisplayName = "Affected And Path Tiles")
};