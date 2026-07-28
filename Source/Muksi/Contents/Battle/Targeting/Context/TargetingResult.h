#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "TargetingResult.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepContext> StepContexts;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> AffectedCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> PathCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;

	const FTargetingStepContext* GetStepContext(int32 StepIndex) const
	{
		return StepContexts.IsValidIndex(StepIndex) ? &StepContexts[StepIndex] : nullptr;
	}

	const FTargetingStepContext* GetLastStepContext() const
	{
		return StepContexts.IsEmpty() ? nullptr : &StepContexts.Last();
	}

	bool HasSelectedCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext && StepContext->HasSelectedCoord();
	}

	FHexOffsetCoord GetOriginCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->OriginCoord : FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	}

	FVector GetOriginWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->OriginWorldLocation : FVector::ZeroVector;
	}

	FHexOffsetCoord GetSelectedCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->SelectedCoord : FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	}

	FVector GetSelectedWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->SelectedWorldLocation : FVector::ZeroVector;
	}

	FVector GetAimWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->AimWorldLocation : FVector::ZeroVector;
	}

	int32 GetDirection() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->Direction : INDEX_NONE;
	}

	ABattleCharacterBase* GetPrimaryTargetCharacter() const
	{
		return TargetCharacters.IsValidIndex(0) ? TargetCharacters[0] : nullptr;
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
		StepContexts.Empty();
		AffectedCoords.Empty();
		PathCoords.Empty();
		TargetCharacters.Empty();
	}
};
