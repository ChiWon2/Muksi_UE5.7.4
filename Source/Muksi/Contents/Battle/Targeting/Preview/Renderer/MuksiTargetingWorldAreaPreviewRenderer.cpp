#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingWorldAreaPreviewRenderer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingWorldAreaPreviewRenderer::Initialize(UStaticMeshComponent* InWorldAreaMesh)
{
	WorldAreaMesh = InWorldAreaMesh;
	Hide();
}

void UMuksiTargetingWorldAreaPreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
{
	switch (Command.WorldAreaStyle)
	{
	case EMuksiCardTargetingWorldAreaPreviewStyle::Circle:
		UpdateCirclePreview(Command, Settings);
		return;

	case EMuksiCardTargetingWorldAreaPreviewStyle::None:
	case EMuksiCardTargetingWorldAreaPreviewStyle::Cone:
	case EMuksiCardTargetingWorldAreaPreviewStyle::Rectangle:
	default:
		Hide();
		return;
	}
}

void UMuksiTargetingWorldAreaPreviewRenderer::Hide()
{
	if (WorldAreaMesh)
	{
		WorldAreaMesh->SetVisibility(false);
	}
}

void UMuksiTargetingWorldAreaPreviewRenderer::UpdateCirclePreview(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
{
	if (!WorldAreaMesh || Command.WorldAreaRadius <= 0.0f)
	{
		Hide();
		return;
	}

	FVector PreviewLocation = Command.AimWorldLocation;
	PreviewLocation.Z += Settings.HeightOffset;

	WorldAreaMesh->SetWorldLocation(PreviewLocation);
	WorldAreaMesh->SetWorldRotation(FRotator::ZeroRotator);
	WorldAreaMesh->SetWorldScale3D(GetPlaneScaleByRadius(Command.WorldAreaRadius));
	WorldAreaMesh->SetHiddenInGame(false);
	WorldAreaMesh->SetVisibility(true, true);
}

FVector UMuksiTargetingWorldAreaPreviewRenderer::GetPlaneScaleByRadius(float Radius) const
{
	if (Radius <= 0.0f)
	{
		return FVector::OneVector;
	}

	const float Diameter = Radius * 2.0f;
	const float UniformScale = Diameter / BasePlaneSize;

	return FVector(UniformScale, UniformScale, 1.0f);
}