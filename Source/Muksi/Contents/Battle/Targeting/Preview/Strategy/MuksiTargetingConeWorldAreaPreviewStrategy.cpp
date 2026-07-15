#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingConeWorldAreaPreviewStrategy.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingWorldAreaPreviewRenderer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingConeWorldAreaPreviewStrategy::Initialize(UStaticMeshComponent* InWorldAreaMesh)
{
	Super::Initialize(InWorldAreaMesh);

	if (WorldAreaMesh)
	{
		ConeMaterialInstance = WorldAreaMesh->CreateDynamicMaterialInstance(0);
	}
}

void UMuksiTargetingConeWorldAreaPreviewStrategy::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
{
	if (!WorldAreaMesh || Command.WorldAreaRadius <= 0.0f || Command.WorldAreaAngle <= 0.0f)
	{
		Hide();
		return;
	}

	FVector AimDirection = Command.AimWorldLocation - Command.SourceWorldLocation;
	AimDirection.Z = 0.0f;

	if (!AimDirection.Normalize())
	{
		Hide();
		return;
	}

	FVector PreviewLocation = Command.SourceWorldLocation;
	PreviewLocation.Z += Settings.HeightOffset;

	const float PreviewYaw = FMath::RadiansToDegrees(FMath::Atan2(AimDirection.Y, AimDirection.X));

	if (ConeMaterialInstance)
	{
		ConeMaterialInstance->SetScalarParameterValue(TEXT("ConeAngle"), FMath::Clamp(Command.WorldAreaAngle, 1.0f, 360.0f));
	}

	WorldAreaMesh->SetWorldLocation(PreviewLocation);
	WorldAreaMesh->SetWorldRotation(FRotator(0.0f, PreviewYaw, 0.0f));
	WorldAreaMesh->SetWorldScale3D(GetPlaneScaleByRadius(Command.WorldAreaRadius));
	WorldAreaMesh->SetHiddenInGame(false);
	WorldAreaMesh->SetVisibility(true, true);
}