#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingGroup.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "TargetingStepResult.generated.h"

USTRUCT(BlueprintType)
struct FTargetingStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FTargetingStep Step;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingGroup> Groups;

	const FTargetingGroup* GetPrimaryGroup() const
	{
		return Groups.IsValidIndex(0) ? &Groups[0] : nullptr;
	}

	FTargetingGroup* GetMutablePrimaryGroup()
	{
		return Groups.IsValidIndex(0) ? &Groups[0] : nullptr;
	}

	TArray<FHexOffsetCoord> GetAllAffectedCoords() const
	{
		TArray<FHexOffsetCoord> Result;

		for (const FTargetingGroup& Group : Groups)
		{
			for (const FHexOffsetCoord& Coord : Group.AffectedCoords)
				Result.AddUnique(Coord);
		}

		return Result;
	}

	TArray<FHexOffsetCoord> GetAllPathCoords() const
	{
		TArray<FHexOffsetCoord> Result;

		for (const FTargetingGroup& Group : Groups)
		{
			for (const FHexOffsetCoord& Coord : Group.PathCoords)
				Result.AddUnique(Coord);
		}

		return Result;
	}

	TArray<ABattleCharacterBase*> GetAllTargets() const
	{
		TArray<ABattleCharacterBase*> Result;

		for (const FTargetingGroup& Group : Groups)
		{
			for (ABattleCharacterBase* Target : Group.Targets)
			{
				if (Target)
					Result.AddUnique(Target);
			}
		}

		return Result;
	}

	bool ContainsTarget(const ABattleCharacterBase* Target) const
	{
		if (!Target)
			return false;

		for (const FTargetingGroup& Group : Groups)
		{
			if (Group.Targets.Contains(Target))
				return true;
		}

		return false;
	}

	void Reset()
	{
		Step.Reset();
		Groups.Empty();
	}
};
