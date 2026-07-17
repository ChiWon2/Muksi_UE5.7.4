#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "TargetingDeveloperSettings.generated.h"

class UMaterialInterface;
class UStaticMesh;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "MuksiTargetingSettings"))
class MUKSI_API UTargetingDeveloperSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UTargetingDeveloperSettings();

public:
    UPROPERTY(Config, EditAnywhere, Category = "Preview|Range")
    TSoftObjectPtr<UMaterialInterface> RangePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Grid")
    TSoftObjectPtr<UMaterialInterface> GridPreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Grid")
    TSoftObjectPtr<UStaticMesh> GridPreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Straight")
    TSoftObjectPtr<UMaterialInterface> StraightPreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Straight")
    TSoftObjectPtr<UStaticMesh> StraightPreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Arrow")
    TSoftObjectPtr<UMaterialInterface> ArrowPreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Arrow")
    TSoftObjectPtr<UStaticMesh> ArrowPreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Arc")
    TSoftObjectPtr<UMaterialInterface> ArcPreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Circle")
    TSoftObjectPtr<UMaterialInterface> CirclePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Circle")
    TSoftObjectPtr<UStaticMesh> CirclePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Cone")
    TSoftObjectPtr<UMaterialInterface> ConePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Cone")
    TSoftObjectPtr<UStaticMesh> ConePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Rectangle")
    TSoftObjectPtr<UMaterialInterface> RectanglePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Rectangle")
    TSoftObjectPtr<UStaticMesh> RectanglePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Spline")
    TSoftObjectPtr<UMaterialInterface> SplinePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewHeightOffset = 5.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewLineThickness = 10.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewMeshBaseSize = 100.0f;
};