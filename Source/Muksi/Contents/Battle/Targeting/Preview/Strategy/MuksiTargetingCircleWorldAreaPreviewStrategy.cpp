#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingCircleWorldAreaPreviewStrategy.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingWorldAreaPreviewRenderer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingCircleWorldAreaPreviewStrategy::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
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