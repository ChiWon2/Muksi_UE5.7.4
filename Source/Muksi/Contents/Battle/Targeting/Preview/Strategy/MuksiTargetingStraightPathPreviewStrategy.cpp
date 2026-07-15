#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingStraightPathPreviewStrategy.h"

#include "Components/StaticMeshComponent.h"

void UMuksiTargetingStraightPathPreviewStrategy::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings)
{
	FVector StartWorldLocation = Command.SourceWorldLocation;
	FVector EndWorldLocation = Command.AimWorldLocation;

	ApplyHeightOffset(StartWorldLocation, EndWorldLocation, Settings.PathPreviewHeightOffset);
	UpdatePathSegment(StraightPathMesh, StartWorldLocation, EndWorldLocation, Settings.PathWidth);

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}
}