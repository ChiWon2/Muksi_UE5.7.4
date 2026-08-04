#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingStepResult.generated.h"

USTRUCT(BlueprintType)
struct FTargetingStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bValid = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;

	bool HasOriginCoord() const
	{
		return OriginCoord.IsValid();
	}

	bool HasSelectedCoord() const
	{
		return SelectedCoord.IsValid();
	}

	bool HasDirection() const
	{
		return Direction != INDEX_NONE;
	}

	void Reset()
	{
		bValid = false;
		OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		Direction = INDEX_NONE;
	}
};
