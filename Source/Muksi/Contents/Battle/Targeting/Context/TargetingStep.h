#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingStep.generated.h"

/** Concrete targeting step for a specific snapshot (selection preview or simulation resolve). */
USTRUCT(BlueprintType)
struct FTargetingStep
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Targeting")
    FHexOffsetCoord OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

    UPROPERTY(BlueprintReadOnly, Category = "Targeting")
    FHexOffsetCoord TargetCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

    UPROPERTY(BlueprintReadOnly, Category = "Targeting")
    int32 Direction = INDEX_NONE;

    bool HasOriginCoord() const { return OriginCoord.IsValid(); }
    bool HasTargetCoord() const { return TargetCoord.IsValid(); }
    bool HasDirection() const { return Direction != INDEX_NONE; }

    void Reset()
    {
        OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
        TargetCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
        Direction = INDEX_NONE;
    }
};
