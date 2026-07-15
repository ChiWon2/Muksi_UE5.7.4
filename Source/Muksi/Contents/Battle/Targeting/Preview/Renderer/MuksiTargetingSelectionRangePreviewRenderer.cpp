#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingSelectionRangePreviewRenderer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingSelectionRangePreviewRenderer::Initialize(UStaticMeshComponent* InRangePreviewMesh)
{
	RangePreviewMesh = InRangePreviewMesh;

	Hide();
}

void UMuksiTargetingSelectionRangePreviewRenderer::SetGridManager(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

void UMuksiTargetingSelectionRangePreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingSelectionRangePreviewSettings& Settings)
{
	if (!RangePreviewMesh || !GridManager)
	{
		Hide();
		return;
	}

	if (!Command.bShowSelectionRange || Command.SelectionRange <= 0)
	{
		Hide();
		return;
	}

	const float RangeWorldRadius = GetSelectionRangeWorldRadius(Command, Settings);

	if (RangeWorldRadius <= 0.0f)
	{
		Hide();
		return;
	}

	FVector PreviewLocation = Command.SourceWorldLocation;

	PreviewLocation.Z += Settings.HeightOffset;

	RangePreviewMesh->SetWorldLocation(PreviewLocation);
	RangePreviewMesh->SetWorldRotation(FRotator::ZeroRotator);
	RangePreviewMesh->SetWorldScale3D(GetPlaneScaleByRadius(RangeWorldRadius));
	RangePreviewMesh->SetHiddenInGame(false);
	RangePreviewMesh->SetVisibility(true, true);
}

void UMuksiTargetingSelectionRangePreviewRenderer::Hide()
{
	if (RangePreviewMesh)
	{
		RangePreviewMesh->SetVisibility(false);
	}
}

float UMuksiTargetingSelectionRangePreviewRenderer::GetSelectionRangeWorldRadius(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingSelectionRangePreviewSettings& Settings) const
{
	if (!GridManager)
	{
		return 0.0f;
	}

	return GridManager->GetWorldRadiusByGridRange(Command.SelectionRange, Settings.bIncludeOuterTileRadius);
}

FVector UMuksiTargetingSelectionRangePreviewRenderer::GetPlaneScaleByRadius(float Radius) const
{
	if (Radius <= 0.0f)
	{
		return FVector::OneVector;
	}

	const float Diameter = Radius * 2.0f;
	const float UniformScale = Diameter / BasePlaneSize;

	return FVector(UniformScale, UniformScale, 1.0f);
}