#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingStepPreviewInstance.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UTargetingPresentationController::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

bool UTargetingPresentationController::StartLivePreview(ABattleCharacterBase* SourceCharacter, EBattleSimulationWorldType GridWorldType)
{
	ClearLivePreview();
	if (!IsValid(SourceCharacter))
	{
		return false;
	}

	LiveSourceCharacter = SourceCharacter;
	LiveGridWorldType = GridWorldType;
	return true;
}

void UTargetingPresentationController::UpdateLivePreview(const FTargetingStepCardData& StepData, const FTargetingPreviewContext& PreviewContext)
{
	if (!LiveSourceCharacter)
	{
		return;
	}

	if (LivePreviewStepData != &StepData)
	{
		DestroyLiveStepPreview();
		if (!CreateLiveStepPreview(StepData))
		{
			RefreshAffectedHighlights();
			return;
		}
	}

	LiveStepPreview->UpdatePreview(PreviewContext);
	RefreshAffectedHighlights();
}

bool UTargetingPresentationController::CreateLiveStepPreview(const FTargetingStepCardData& StepData)
{
	const FTargetingPhasePresentationSettings& PresentationSettings = StepData.Presentation.Phases.Targeting;
	LiveStepPreview = NewObject<UTargetingStepPreviewInstance>(this);
	if (!LiveStepPreview
		|| !LiveStepPreview->Initialize(
			LiveSourceCharacter.Get(),
			GridManager.Get(),
			LiveGridWorldType,
			StepData,
			PresentationSettings))
	{
		LiveStepPreview = nullptr;
		LivePreviewStepData = nullptr;
		return false;
	}

	LivePreviewStepData = &StepData;
	return true;
}

void UTargetingPresentationController::DestroyLiveStepPreview()
{
	if (LiveStepPreview)
	{
		LiveStepPreview->EndPresentation();
	}

	LiveStepPreview = nullptr;
	LivePreviewStepData = nullptr;
}

void UTargetingPresentationController::ClearLivePreview()
{
	DestroyLiveStepPreview();
	LiveSourceCharacter = nullptr;
	LiveGridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
	RefreshAffectedHighlights();
}

bool UTargetingPresentationController::AddTargetingResultPreview(ABattleCharacterBase* SourceCharacter, EBattleSimulationWorldType GridWorldType, const FTargetingCardData& CardData, const FTargetingResult& TargetingResult, int32 StepIndex, const FTargetingPhasePresentationSettings& PresentationSettings, bool bEnemyStyle)
{
	if (!IsValid(SourceCharacter)) return false;

	const FTargetingStepCardData* StepData = CardData.GetStep(StepIndex);
	const FTargetingStepResult* StepResult = TargetingResult.GetStep(StepIndex);
	if (!StepData || !StepResult) return false;

	UTargetingStepPreviewInstance* PreviewInstance = NewObject<UTargetingStepPreviewInstance>(this);
	if (!PreviewInstance
		|| !PreviewInstance->Initialize(SourceCharacter, GridManager.Get(), GridWorldType, *StepData, PresentationSettings, bEnemyStyle))
	{
		return false;
	}

	FTargetingPreviewContext PreviewContext;
	PreviewContext.GridManager = GridManager.Get();
	PreviewContext.StepData = StepData;
	PreviewContext.TargetingStep = StepResult;
	PreviewInstance->UpdatePreview(PreviewContext);

	TargetingResultPreviewInstances.Add(PreviewInstance);
	RefreshAffectedHighlights();
	return true;
}

void UTargetingPresentationController::ClearTargetingResultPreviews()
{
	for (UTargetingStepPreviewInstance* PreviewInstance : TargetingResultPreviewInstances)
	{
		if (PreviewInstance)
		{
			PreviewInstance->EndPresentation();
		}
	}

	TargetingResultPreviewInstances.Empty();
	RefreshAffectedHighlights();
}

void UTargetingPresentationController::RefreshAffectedHighlights()
{
	TArray<FHexOffsetCoord> Coords;

	if (LiveStepPreview)
	{
		for (const FHexOffsetCoord& Coord : LiveStepPreview->GetAffectedHighlightCoords())
		{
			Coords.AddUnique(Coord);
		}
	}

	for (const UTargetingStepPreviewInstance* PreviewInstance : TargetingResultPreviewInstances)
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
	ClearLivePreview();
	ClearTargetingResultPreviews();
}
