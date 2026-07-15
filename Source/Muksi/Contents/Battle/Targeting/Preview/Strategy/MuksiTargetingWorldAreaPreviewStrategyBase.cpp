#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingWorldAreaPreviewStrategyBase.h"

#include "Components/StaticMeshComponent.h"

void UMuksiTargetingWorldAreaPreviewStrategyBase::Initialize(UStaticMeshComponent* InWorldAreaMesh)
{
	WorldAreaMesh = InWorldAreaMesh;
	Hide();
}

void UMuksiTargetingWorldAreaPreviewStrategyBase::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
{
}

void UMuksiTargetingWorldAreaPreviewStrategyBase::Hide()
{
	if (WorldAreaMesh)
	{
		WorldAreaMesh->SetVisibility(false);
	}
}

FVector UMuksiTargetingWorldAreaPreviewStrategyBase::GetPlaneScaleByRadius(float Radius) const
{
	if (Radius <= 0.0f)
	{
		return FVector::OneVector;
	}

	const float Diameter = Radius * 2.0f;
	const float UniformScale = Diameter / BasePlaneSize;

	return FVector(UniformScale, UniformScale, 1.0f);
}