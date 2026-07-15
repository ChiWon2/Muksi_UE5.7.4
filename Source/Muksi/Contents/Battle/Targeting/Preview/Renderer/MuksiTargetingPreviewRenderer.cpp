#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingPreviewRenderer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingGridPreviewRenderer.h"

void UMuksiTargetingPreviewRenderer::Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, ABattleGridManager* InGridManager, UStaticMeshComponent* InRangePreviewMesh, UStaticMeshComponent* InWorldAreaMesh, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline)
{
	SelectionRangeRenderer = NewObject<UMuksiTargetingSelectionRangePreviewRenderer>(this);
	WorldAreaRenderer = NewObject<UMuksiTargetingWorldAreaPreviewRenderer>(this);
	GridRenderer = NewObject<UMuksiTargetingGridPreviewRenderer>(this);
	PathRenderer = NewObject<UMuksiTargetingPathPreviewRenderer>(this);

	if (SelectionRangeRenderer)
	{
		SelectionRangeRenderer->Initialize(InRangePreviewMesh);
	}

	if (WorldAreaRenderer)
	{
		WorldAreaRenderer->Initialize(InWorldAreaMesh);
	}

	if (PathRenderer)
	{
		PathRenderer->Initialize(InOwnerActor, InSceneRoot, InStraightPathMesh, InArrowMesh, InPathSpline);
	}

	SetGridManager(InGridManager);
	Hide();
}

void UMuksiTargetingPreviewRenderer::SetGridManager(ABattleGridManager* InGridManager)
{
	if (SelectionRangeRenderer)
	{
		SelectionRangeRenderer->SetGridManager(InGridManager);
	}

	if (GridRenderer)
	{
		GridRenderer->SetGridManager(InGridManager);
	}
}

void UMuksiTargetingPreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPreviewSettings& Settings)
{
	if (SelectionRangeRenderer)
	{
		SelectionRangeRenderer->Update(Command, Settings.SelectionRangeSettings);
	}

	if (WorldAreaRenderer)
	{
		WorldAreaRenderer->Update(Command, Settings.WorldAreaSettings);
	}

	if (PathRenderer)
	{
		PathRenderer->Update(Command, Settings.PathSettings);
	}

	if (GridRenderer)
	{
		GridRenderer->Update(Command);
	}
}

void UMuksiTargetingPreviewRenderer::Hide()
{
	if (SelectionRangeRenderer)
	{
		SelectionRangeRenderer->Hide();
	}

	if (WorldAreaRenderer)
	{
		WorldAreaRenderer->Hide();
	}

	if (PathRenderer)
	{
		PathRenderer->Hide();
	}

	if (GridRenderer)
	{
		GridRenderer->Hide();
	}
}