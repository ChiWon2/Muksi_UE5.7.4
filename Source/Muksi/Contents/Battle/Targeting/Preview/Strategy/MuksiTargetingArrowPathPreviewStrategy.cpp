#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingArrowPathPreviewStrategy.h"

#include "Components/StaticMeshComponent.h"

void UMuksiTargetingArrowPathPreviewStrategy::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings)
{
	if (!ArrowMesh)
	{
		return;
	}

	FVector StartWorldLocation = Command.SourceWorldLocation;
	FVector EndWorldLocation = Command.AimWorldLocation;

	ApplyHeightOffset(StartWorldLocation, EndWorldLocation, Settings.PathPreviewHeightOffset);

	const FVector FullDirection = EndWorldLocation - StartWorldLocation;
	const float FullLength = FullDirection.Size();

	if (FullLength <= KINDA_SMALL_NUMBER)
	{
		Hide();
		return;
	}

	const FVector Direction = FullDirection / FullLength;
	const float VisibleArrowLength = FMath::Min(Settings.ArrowHeadLength, FullLength);
	const FVector ArrowBaseWorldLocation = EndWorldLocation - Direction * VisibleArrowLength;
	const FVector ArrowCenterWorldLocation = EndWorldLocation - Direction * VisibleArrowLength * 0.5f;
	const FRotator PathRotation = Direction.Rotation();
	const float ArrowLengthScale = VisibleArrowLength / BaseArrowMeshLength;
	const float ArrowWidthScale = Settings.ArrowHeadWidth / BaseArrowMeshWidth;

	if (FullLength > VisibleArrowLength)
	{
		UpdatePathSegment(StraightPathMesh, StartWorldLocation, ArrowBaseWorldLocation, Settings.PathWidth);
	}
	else if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}

	ArrowMesh->SetWorldLocation(ArrowCenterWorldLocation);
	ArrowMesh->SetWorldRotation(PathRotation);
	ArrowMesh->SetWorldScale3D(FVector(ArrowLengthScale, ArrowWidthScale, 1.0f));
	ArrowMesh->SetHiddenInGame(false);
	ArrowMesh->SetVisibility(true, true);
}