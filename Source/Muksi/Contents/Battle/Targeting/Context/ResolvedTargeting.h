#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"
#include "ResolvedTargeting.generated.h"

USTRUCT(BlueprintType)
struct FResolvedTargeting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepResult> Steps;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> AffectedCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> PathCoords;

	bool IsResolved() const
	{
		return !Steps.IsEmpty() || !AffectedCoords.IsEmpty() || !PathCoords.IsEmpty();
	}

	const FTargetingStepResult* GetStep(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr;
	}

	const FTargetingStepResult* GetLastStep() const
	{
		return Steps.IsEmpty() ? nullptr : &Steps.Last();
	}

	bool HasSelectedCoord() const
	{
		const FTargetingStepResult* Step = GetLastStep();
		return Step && Step->HasSelectedCoord();
	}

	FHexOffsetCoord GetOriginCoord() const
	{
		const FTargetingStepResult* Step = GetLastStep();
		return Step ? Step->OriginCoord : FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	}

	FHexOffsetCoord GetSelectedCoord() const
	{
		const FTargetingStepResult* Step = GetLastStep();
		return Step ? Step->SelectedCoord : FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	}

	int32 GetDirection() const
	{
		const FTargetingStepResult* Step = GetLastStep();
		return Step ? Step->Direction : INDEX_NONE;
	}

	void AddAffectedCoord(const FHexOffsetCoord& Coord)
	{
		AffectedCoords.AddUnique(Coord);
	}

	void AddPathCoord(const FHexOffsetCoord& Coord)
	{
		PathCoords.AddUnique(Coord);
	}

	void Reset()
	{
		Steps.Empty();
		AffectedCoords.Empty();
		PathCoords.Empty();
	}
};
