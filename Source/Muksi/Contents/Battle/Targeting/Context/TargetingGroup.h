#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingGroup.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> AffectedCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> PathCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> Targets;

	void Reset()
	{
		Direction = INDEX_NONE;
		AffectedCoords.Empty();
		PathCoords.Empty();
		Targets.Empty();
	}
};
