#include "Muksi/Contents/Battle/Targeting/Preview/Area/PointAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UPointAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
		return;

	PointPreviewMesh = Settings->CirclePreviewMesh.LoadSynchronous();
	PointPreviewMaterial = Settings->CirclePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UPointAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.TargetingStep)
		return;

	const FTargetingGroup* Group = Context.TargetingStep->GetPrimaryGroup();

	if (!Group || Group->AffectedCoords.IsEmpty())
		return;

	const FHexOffsetCoord PointCoord = Context.HasTargetCoord() ? Context.GetTargetCoord() : Group->AffectedCoords[0];
	FVector CenterLocation = FVector::ZeroVector;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(PointCoord, CenterLocation))
		return;

	UStaticMeshComponent* PreviewMeshComponent = GetPreviewActor()->GetAreaPreviewMesh();

	if (!PreviewMeshComponent || !PointPreviewMesh)
		return;

	const float WorldRadius = Context.GridManager->GetWorldRadiusByGridRange(0, true);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
		return;

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = CenterLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);

	PreviewMeshComponent->SetStaticMesh(PointPreviewMesh);

	if (PointPreviewMaterial)
		PreviewMeshComponent->SetMaterial(0, PointPreviewMaterial);

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}
