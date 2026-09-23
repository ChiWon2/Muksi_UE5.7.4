#include "Muksi/Contents/Battle/Targeting/Preview/Area/CircleAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UCircleAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
		return;

	CirclePreviewMesh = Settings->CirclePreviewMesh.LoadSynchronous();
	CirclePreviewMaterial = Settings->CirclePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UCircleAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.TargetingStep)
		return;

	const FTargetingGroup* Group = Context.TargetingStep->GetPrimaryGroup();

	if (!Group || Group->AffectedCoords.IsEmpty())
		return;

	const FHexOffsetCoord CenterCoord = Context.HasTargetCoord() ? Context.GetTargetCoord() : Group->AffectedCoords[0];
	FVector CenterLocation = FVector::ZeroVector;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(CenterCoord, CenterLocation))
		return;

	UStaticMeshComponent* PreviewMeshComponent = GetPreviewActor()->GetAreaPreviewMesh();

	if (!PreviewMeshComponent || !CirclePreviewMesh)
		return;

	const float WorldRadius = CalculateWorldRadius(Context, *Group);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
		return;

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = CenterLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);

	PreviewMeshComponent->SetStaticMesh(CirclePreviewMesh);

	if (CirclePreviewMaterial)
		PreviewMeshComponent->SetMaterial(0, CirclePreviewMaterial);

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}

float UCircleAreaPreviewVisualizer::CalculateWorldRadius(
	const FTargetingPreviewContext& Context,
	const FTargetingGroup& Group) const
{
	if (!Context.GridManager || Group.AffectedCoords.IsEmpty())
		return 0.0f;

	const FHexOffsetCoord CenterCoord = Context.HasTargetCoord() ? Context.GetTargetCoord() : Group.AffectedCoords[0];
	int32 GridRange = 0;

	for (const FHexOffsetCoord& Coord : Group.AffectedCoords)
		GridRange = FMath::Max(GridRange, FHexGridMath::GetHexDistance(CenterCoord, Coord));

	return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(0, GridRange), true);
}
