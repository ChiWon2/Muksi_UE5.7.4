#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedStepResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/SelectionStepResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"

class ABattleGridManager;
struct FTargetingStepCardData;

struct FTargetingPreviewContext
{
	ABattleGridManager* GridManager = nullptr;
	const FTargetingStepCardData* StepData = nullptr;
	const FSelectionStepResult* SelectionStep = nullptr;
	const FTargetingStepResult* TargetingStep = nullptr;
	const TArray<FHexOffsetCoord>* PreviewAffectedCoords = nullptr;
	const TArray<FHexOffsetCoord>* PreviewPathCoords = nullptr;

	bool IsValid() const { return GridManager && StepData; }
	bool IsStepValid() const { return SelectionStep ? SelectionStep->bValid : TargetingStep != nullptr; }

	bool HasOriginCoord() const
	{
		return SelectionStep ? SelectionStep->HasOriginCoord() : TargetingStep && TargetingStep->ResolvedStep.HasOriginCoord();
	}

	FHexOffsetCoord GetOriginCoord() const
	{
		if (SelectionStep) return SelectionStep->OriginCoord;
		return TargetingStep ? TargetingStep->ResolvedStep.OriginCoord : FHexOffsetCoord::Invalid();
	}

	bool HasTargetCoord() const
	{
		return SelectionStep ? SelectionStep->HasSelectedCoord() : TargetingStep && TargetingStep->ResolvedStep.HasResolvedCoord();
	}

	FHexOffsetCoord GetTargetCoord() const
	{
		if (SelectionStep) return SelectionStep->SelectedCoord;
		return TargetingStep ? TargetingStep->ResolvedStep.ResolvedCoord : FHexOffsetCoord::Invalid();
	}

	bool HasDirection() const
	{
		return SelectionStep ? SelectionStep->HasSelectedDirection() : TargetingStep && TargetingStep->ResolvedStep.HasResolvedDirection();
	}

	int32 GetDirection() const
	{
		if (SelectionStep) return SelectionStep->SelectedDirection;
		return TargetingStep ? TargetingStep->ResolvedStep.ResolvedDirection : INDEX_NONE;
	}

	const TArray<FHexOffsetCoord>* GetAffectedCoords() const
	{
		if (TargetingStep) return &TargetingStep->AffectedCoords;
		return PreviewAffectedCoords;
	}

	const TArray<FHexOffsetCoord>* GetPathCoords() const
	{
		if (TargetingStep) return &TargetingStep->PathCoords;
		return PreviewPathCoords;
	}
};
