#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingStepPreviewInstance.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UTargetingPresentationController::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

bool UTargetingPresentationController::PresentCurrentStep(const FTargetingPreviewContext& PreviewContext)
{
	if (!PreviewContext.IsValid())
	{
		ClearCurrentStepPreview();
		return false;
	}

	const bool bNeedsNewInstance =
		!CurrentStepPreview
		|| CurrentStepSourceCharacter != PreviewContext.SourceCharacter
		|| CurrentStepData != PreviewContext.StepData
		|| CurrentPresentationSettings != PreviewContext.PresentationSettings;

	if (bNeedsNewInstance)
	{
		DestroyCurrentStepPreview();
		if (!CreateCurrentStepPreview(PreviewContext))
		{
			RefreshAffectedHighlights();
			return false;
		}
	}

	CurrentStepPreview->UpdatePreview(PreviewContext);
	RefreshAffectedHighlights();
	return true;
}

bool UTargetingPresentationController::CreateCurrentStepPreview(const FTargetingPreviewContext& PreviewContext)
{
	CurrentStepPreview = NewObject<UTargetingStepPreviewInstance>(this);
	if (!CurrentStepPreview || !CurrentStepPreview->Initialize(PreviewContext))
	{
		CurrentStepPreview = nullptr;
		return false;
	}

	CurrentStepSourceCharacter = PreviewContext.SourceCharacter;
	CurrentStepData = PreviewContext.StepData;
	CurrentPresentationSettings = PreviewContext.PresentationSettings;
	return true;
}

void UTargetingPresentationController::DestroyCurrentStepPreview()
{
	if (CurrentStepPreview)
	{
		CurrentStepPreview->EndPresentation();
	}

	CurrentStepPreview = nullptr;
	CurrentStepSourceCharacter = nullptr;
	CurrentStepData = nullptr;
	CurrentPresentationSettings = nullptr;
}

void UTargetingPresentationController::ClearCurrentStepPreview()
{
	DestroyCurrentStepPreview();
	RefreshAffectedHighlights();
}

bool UTargetingPresentationController::AddStepPreview(const FTargetingPreviewContext& PreviewContext)
{
	if (!PreviewContext.IsValid())
	{
		return false;
	}

	UTargetingStepPreviewInstance* PreviewInstance = NewObject<UTargetingStepPreviewInstance>(this);
	if (!PreviewInstance || !PreviewInstance->Initialize(PreviewContext))
	{
		return false;
	}

	PreviewInstance->UpdatePreview(PreviewContext);
	StepPreviews.Add(PreviewInstance);
	RefreshAffectedHighlights();
	return true;
}

void UTargetingPresentationController::DestroyStepPreviews()
{
	for (UTargetingStepPreviewInstance* PreviewInstance : StepPreviews)
	{
		if (PreviewInstance)
		{
			PreviewInstance->EndPresentation();
		}
	}

	StepPreviews.Empty();
}

void UTargetingPresentationController::ClearStepPreviews()
{
	DestroyStepPreviews();
	RefreshAffectedHighlights();
}

void UTargetingPresentationController::RefreshAffectedHighlights()
{
	TArray<FHexOffsetCoord> Coords;

	if (CurrentStepPreview)
	{
		for (const FHexOffsetCoord& Coord : CurrentStepPreview->GetAffectedHighlightCoords())
		{
			Coords.AddUnique(Coord);
		}
	}

	for (const UTargetingStepPreviewInstance* PreviewInstance : StepPreviews)
	{
		if (!PreviewInstance)
		{
			continue;
		}

		for (const FHexOffsetCoord& Coord : PreviewInstance->GetAffectedHighlightCoords())
		{
			Coords.AddUnique(Coord);
		}
	}

	if (Coords.IsEmpty())
	{
		GridManager->ClearAllTargetIndicators();
	}
	else
	{
		GridManager->SetTargetIndicators(Coords);
	}
}

void UTargetingPresentationController::ClearAllPresentation()
{
	DestroyCurrentStepPreview();
	DestroyStepPreviews();
	RefreshAffectedHighlights();
}
