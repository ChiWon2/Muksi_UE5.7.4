#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingStepPreviewInstance.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UTargetingStepPreviewInstance::BeginDestroy()
{
	EndPresentation();
	Super::BeginDestroy();
}

bool UTargetingStepPreviewInstance::Initialize(const FTargetingPreviewContext& PreviewContext)
{
	EndPresentation();

	if (!PreviewContext.IsValid())
	{
		return false;
	}

	SourceCharacter = PreviewContext.SourceCharacter;
	GridManager = PreviewContext.GridManager;
	bShowAffectedHighlight = PreviewContext.PresentationSettings->bShowAffectedHighlight;

	if (!EnsurePreviewActor())
	{
		EndPresentation();
		return false;
	}

	CreateVisualizers(*PreviewContext.StepData, *PreviewContext.PresentationSettings);
	return true;
}

void UTargetingStepPreviewInstance::UpdatePreview(const FTargetingPreviewContext& PreviewContext)
{
	if (!PreviewActor || !PreviewContext.IsValid())
	{
		return;
	}

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	if (ActivePathPreviewVisualizer && PreviewContext.IsStepValid())
	{
		ActivePathPreviewVisualizer->UpdatePreview(PreviewContext);
	}
	else if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->ClearPreview();
	}

	AffectedHighlightCoords.Reset();

	if (bShowAffectedHighlight)
	{
		if (PreviewContext.TargetingStep)
		{
			for (const FTargetingGroup& Group : PreviewContext.TargetingStep->Groups)
			{
				for (const FHexOffsetCoord& Coord : Group.AffectedCoords)
					AffectedHighlightCoords.AddUnique(Coord);
			}
		}
		else if (const TArray<FHexOffsetCoord>* AffectedCoords = PreviewContext.GetAffectedCoords())
		{
			for (const FHexOffsetCoord& Coord : *AffectedCoords)
				AffectedHighlightCoords.AddUnique(Coord);
		}
	}
}

void UTargetingStepPreviewInstance::EndPresentation()
{
	ClearVisualizers();
	AffectedHighlightCoords.Reset();
	bShowAffectedHighlight = false;

	if (IsValid(PreviewActor.Get()))
	{
		PreviewActor->Destroy();
	}

	PreviewActor = nullptr;
	SourceCharacter = nullptr;
	GridManager = nullptr;
}

bool UTargetingStepPreviewInstance::EnsurePreviewActor()
{
	if (IsValid(PreviewActor.Get()))
	{
		return true;
	}

	if (!IsValid(SourceCharacter.Get()) || !IsValid(GridManager.Get()))
	{
		return false;
	}

	UWorld* World = SourceCharacter->GetWorld();
	if (!World)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SourceCharacter.Get();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	PreviewActor = World->SpawnActor<ATargetingPreviewActor>(ATargetingPreviewActor::StaticClass(), FTransform::Identity, SpawnParameters);

	if (!IsValid(PreviewActor.Get()))
	{
		return false;
	}

	PreviewActor->Initialize(GridManager.Get());
	return true;
}

void UTargetingStepPreviewInstance::CreateVisualizers(
	const FTargetingStepCardData& StepData,
	const FTargetingPhasePresentationSettings& PresentationSettings)
{
	ClearVisualizers();

	if (PresentationSettings.bShowSelection
		&& StepData.Presentation.Visualizers.Selection.Visualizer
		&& !StepData.Presentation.Visualizers.Selection.Visualizer->HasAnyClassFlags(CLASS_Abstract))
	{
		ActiveSelectionPreviewVisualizer = NewObject<USelectionPreviewVisualizer>(this, StepData.Presentation.Visualizers.Selection.Visualizer);
		ActiveSelectionPreviewVisualizer->Initialize(PreviewActor.Get());
	}

	if (PresentationSettings.bShowPath
		&& StepData.Presentation.Visualizers.Path.Visualizer
		&& !StepData.Presentation.Visualizers.Path.Visualizer->HasAnyClassFlags(CLASS_Abstract))
	{
		ActivePathPreviewVisualizer = NewObject<UPathPreviewVisualizer>(this, StepData.Presentation.Visualizers.Path.Visualizer);
		ActivePathPreviewVisualizer->Initialize(PreviewActor.Get());
	}
}

void UTargetingStepPreviewInstance::ClearVisualizers()
{
	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->ClearPreview();
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->ClearPreview();
	}

	ActiveSelectionPreviewVisualizer = nullptr;
	ActivePathPreviewVisualizer = nullptr;
}
