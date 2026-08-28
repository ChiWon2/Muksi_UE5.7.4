#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "ResolvedStepResult.generated.h"

USTRUCT(BlueprintType)
struct FResolvedStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord ResolvedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 ResolvedDirection = INDEX_NONE;

	bool HasOriginCoord() const { return OriginCoord.IsValid(); }
	bool HasResolvedCoord() const { return ResolvedCoord.IsValid(); }
	bool HasResolvedDirection() const { return ResolvedDirection != INDEX_NONE; }

	void Reset()
	{
		OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		ResolvedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		ResolvedDirection = INDEX_NONE;
	}
};
