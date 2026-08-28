#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedStepResult.h"
#include "TargetingStepResult.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FResolvedStepResult ResolvedStep;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> AffectedCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> PathCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> Targets;

	void Reset()
	{
		ResolvedStep.Reset();
		AffectedCoords.Empty();
		PathCoords.Empty();
		Targets.Empty();
	}
};
