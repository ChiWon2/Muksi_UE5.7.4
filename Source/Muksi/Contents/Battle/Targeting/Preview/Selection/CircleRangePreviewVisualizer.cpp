#include "Muksi/Contents/Battle/Targeting/Preview/Selection/CircleRangePreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameters.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelectionData.h"

void UCircleRangePreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
    Super::Initialize(InPreviewActor);

    const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

    if (!Settings)
        return;

    SelectionRangePreviewMesh = Settings->SelectionRangePreviewMesh.LoadSynchronous();
    SelectionRangePreviewMaterial = Settings->SelectionRangePreviewMaterial.LoadSynchronous();
    SelectionRangePreviewMID = nullptr;
    BaseBorderThickness = 0.012f;
    PreviewHeightOffset = Settings->PreviewHeightOffset;
    PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);

    if (SelectionRangePreviewMaterial)
    {
        SelectionRangePreviewMaterial->GetScalarParameterValue(FMaterialParameterInfo(TEXT("BorderThickness"), EMaterialParameterAssociation::GlobalParameter, INDEX_NONE), BaseBorderThickness);
        SelectionRangePreviewMID = UMaterialInstanceDynamic::Create(SelectionRangePreviewMaterial, this);
    }
}

void UCircleRangePreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
    if (!HasPreviewActor() || !Context.IsValid() || !Context.HasOriginCoord() || !SelectionRangePreviewMesh)
    {
        ClearPreview();
        return;
    }

    UStaticMeshComponent* PreviewMesh = GetPreviewActor()->GetSelectionPreviewMesh();

    if (!PreviewMesh)
        return;

    const float WorldRadius = CalculateWorldRadius(Context);

    if (WorldRadius <= KINDA_SMALL_NUMBER)
    {
        ClearPreview();
        return;
    }

    FVector OriginLocation = FVector::ZeroVector;

    if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetOriginCoord(), OriginLocation))
    {
        ClearPreview();
        return;
    }

    const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
    PreviewMesh->SetStaticMesh(SelectionRangePreviewMesh);

    if (SelectionRangePreviewMID)
    {
        const float BorderThickness = FMath::Max(0.0f, BaseBorderThickness) / FMath::Max(KINDA_SMALL_NUMBER, PreviewScale);
        SelectionRangePreviewMID->SetScalarParameterValue(TEXT("BorderThickness"), BorderThickness);
        PreviewMesh->SetMaterial(0, SelectionRangePreviewMID);
    }
    else if (SelectionRangePreviewMaterial)
        PreviewMesh->SetMaterial(0, SelectionRangePreviewMaterial);

    PreviewMesh->SetWorldLocation(OriginLocation + FVector(0.0f, 0.0f, PreviewHeightOffset));
    PreviewMesh->SetWorldRotation(FRotator::ZeroRotator);
    PreviewMesh->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
    PreviewMesh->SetVisibility(true);
}

float UCircleRangePreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context) const
{
    const FTileSelectionData* Data = Context.StepData->Selection.RuleData.GetPtr<FTileSelectionData>();

    if (!Data)
        return 0.0f;

    return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(0, Data->SelectionRange), true);
}
