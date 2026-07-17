#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "ConeAreaPreviewVisualizer.generated.h"

class ATargetingPreviewActor;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UConeAreaPreviewVisualizer : public UAreaPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor) override;
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const override;

private:
	float CalculateWorldRadius(const FTargetingPreviewContext& Context, const FVector& OriginLocation) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> ConePreviewMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> ConePreviewMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ConeDynamicMaterial = nullptr;

	float PreviewHeightOffset = 5.0f;
	float PreviewMeshBaseSize = 100.0f;
};