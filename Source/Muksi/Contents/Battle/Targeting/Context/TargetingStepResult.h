#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "TargetingStepResult.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FTargetingStep Step;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> AffectedCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> PathCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> Targets;

	void Reset()
	{
		Step.Reset();
		AffectedCoords.Empty();
		PathCoords.Empty();
		Targets.Empty();
	}
};
