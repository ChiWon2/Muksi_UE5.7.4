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

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Selection Range")
    TSoftObjectPtr<UMaterialInterface> SelectionRangePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Selection Range")
    TSoftObjectPtr<UStaticMesh> SelectionRangePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Circle")
    TSoftObjectPtr<UMaterialInterface> CirclePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Circle")
    TSoftObjectPtr<UStaticMesh> CirclePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Cone")
    TSoftObjectPtr<UMaterialInterface> ConePreviewMaterial;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Cone")
    TSoftObjectPtr<UStaticMesh> ConePreviewMesh;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewHeightOffset = 5.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewLineThickness = 10.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Preview|Common", meta = (ClampMin = "0.0"))
    float PreviewMeshBaseSize = 100.0f;
};