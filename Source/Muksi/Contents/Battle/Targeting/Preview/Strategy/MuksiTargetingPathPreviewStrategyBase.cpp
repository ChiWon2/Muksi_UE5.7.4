#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingPathPreviewStrategyBase.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

void UMuksiTargetingPathPreviewStrategyBase::Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline)
{
	OwnerActor = InOwnerActor;
	SceneRoot = InSceneRoot;
	StraightPathMesh = InStraightPathMesh;
	ArrowMesh = InArrowMesh;
	PathSpline = InPathSpline;
	Hide();
}

void UMuksiTargetingPathPreviewStrategyBase::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings)
{
}

void UMuksiTargetingPathPreviewStrategyBase::Hide()
{
	if (PathSpline)
	{
		PathSpline->ClearSplinePoints();
		PathSpline->SetVisibility(false);
	}

	if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}
}

void UMuksiTargetingPathPreviewStrategyBase::ApplyHeightOffset(FVector& StartWorldLocation, FVector& EndWorldLocation, float HeightOffset) const
{
	StartWorldLocation.Z += HeightOffset;
	EndWorldLocation.Z += HeightOffset;
}

void UMuksiTargetingPathPreviewStrategyBase::UpdatePathSegment(UStaticMeshComponent* SegmentMesh, const FVector& StartWorldLocation, const FVector& EndWorldLocation, float PathWidth, float LengthMultiplier) const
{
	if (!SegmentMesh)
	{
		return;
	}

	const FVector PathDirection = EndWorldLocation - StartWorldLocation;
	const float PathLength = PathDirection.Size();

	if (PathLength <= KINDA_SMALL_NUMBER)
	{
		SegmentMesh->SetVisibility(false);
		return;
	}

	const FVector MidWorldLocation = (StartWorldLocation + EndWorldLocation) * 0.5f;
	const FRotator PathRotation = PathDirection.Rotation();
	const float LengthScale = PathLength * LengthMultiplier / BasePathMeshLength;
	const float WidthScale = PathWidth / BasePathMeshWidth;

	SegmentMesh->SetWorldLocation(MidWorldLocation);
	SegmentMesh->SetWorldRotation(PathRotation);
	SegmentMesh->SetWorldScale3D(FVector(LengthScale, WidthScale, 1.0f));
	SegmentMesh->SetHiddenInGame(false);
	SegmentMesh->SetVisibility(true, true);
}