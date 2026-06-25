#pragma once

#include "CoreMinimal.h"
#include "RegionID.generated.h"

UENUM(BlueprintType)
enum class ERegionID : uint8
{
    None = 0,

    RED = 1,
    GREEN = 2,
    BLUE,
    YELLO,
    TempRegionName_5,
    TempRegionName_6,
    TempRegionName_7
};