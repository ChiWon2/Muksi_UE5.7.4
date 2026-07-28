#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionData.h"

#include "TileSelectionData.generated.h"

USTRUCT(BlueprintType)
struct FTileSelectionData : public FTargetSelectionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Selection", meta = (ClampMin = "0"))
	int32 SelectionRange = 1;
};