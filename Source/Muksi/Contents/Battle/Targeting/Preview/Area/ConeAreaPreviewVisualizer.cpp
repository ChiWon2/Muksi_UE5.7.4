#include "Muksi/Contents/Battle/Targeting/Preview/Area/ConeAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UConeAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
		return;

	ConePreviewMesh = Settings->ConePreviewMesh.LoadSynchronous();
	ConePreviewMaterial = Settings->ConePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);

	if (ConePreviewMaterial)
		ConeDynamicMaterial = UMaterialInstanceDynamic::Create(ConePreviewMaterial, this);
}

void UConeAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.TargetingStep)
		return;

	const FConePatternData* Data = Context.StepData->Pattern.PatternData.GetPtr<FConePatternData>();
	const FTargetingGroup* Group = Context.TargetingStep->GetPrimaryGroup();

	if (!Data || !Group || !Context.HasOriginCoord())
		return;

	const int32 Direction = Group->Direction != INDEX_NONE ? Group->Direction : Context.GetDirection();

	if (Direction == INDEX_NONE || !Context.GridManager->IsValidCoord(Context.GetOriginCoord()))
		return;

	const FVector OriginLocation = Context.GridManager->GetWorldLocationByCoord(Context.GetOriginCoord());
	const FHexOffsetCoord DirectionCoord = FHexGridMath::GetNeighborCoord(Context.GetOriginCoord(), Direction);
	FVector DirectionVector = Context.GridManager->GetWorldLocationByCoord(DirectionCoord) - OriginLocation;
	DirectionVector.Z = 0.0f;

	if (!DirectionVector.Normalize())
		return;

	UStaticMeshComponent* PreviewMeshComponent = GetPreviewActor()->GetAreaPreviewMesh();

	if (!PreviewMeshComponent || !ConePreviewMesh)
		return;

	const float WorldRadius = CalculateWorldRadius(Context, *Group);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
		return;

	FVector PresentationOriginLocation = FVector::ZeroVector;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetOriginCoord(), PresentationOriginLocation))
		return;

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = PresentationOriginLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);
	const FRotator PreviewRotation(0.0f, DirectionVector.Rotation().Yaw, 0.0f);

	PreviewMeshComponent->SetStaticMesh(ConePreviewMesh);

	if (ConeDynamicMaterial)
	{
		ConeDynamicMaterial->SetScalarParameterValue(TEXT("ConeAngle"), FMath::Clamp(Data->Angle, 1.0f, 360.0f));
		PreviewMeshComponent->SetMaterial(0, ConeDynamicMaterial);
	}
	else if (ConePreviewMaterial)
	{
		PreviewMeshComponent->SetMaterial(0, ConePreviewMaterial);
	}

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(PreviewRotation);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}

float UConeAreaPreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context, const FTargetingGroup& Group) const
{
	if (!Context.GridManager || !Context.HasOriginCoord())
		return 0.0f;

	int32 GridRange = 0;

	for (const FHexOffsetCoord& Coord : Group.AffectedCoords)
		GridRange = FMath::Max(GridRange, FHexGridMath::GetHexDistance(Context.GetOriginCoord(), Coord));

	return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(1, GridRange), true);
}
