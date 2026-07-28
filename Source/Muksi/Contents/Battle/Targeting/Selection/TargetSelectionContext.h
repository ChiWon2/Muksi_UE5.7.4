#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingInputContext.h"

#include "TargetSelectionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FTargetSelectionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FTargetingInputContext InputContext;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 StepIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector OriginWorldLocation = FVector::ZeroVector;

	bool HasOriginCoord() const
	{
		return OriginCoord.X != INDEX_NONE && OriginCoord.Y != INDEX_NONE;
	}

	void Reset()
	{
		SourceCharacter = nullptr;
		GridManager = nullptr;
		InputContext.Reset();
		StepIndex = INDEX_NONE;
		OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		OriginWorldLocation = FVector::ZeroVector;
	}
};
