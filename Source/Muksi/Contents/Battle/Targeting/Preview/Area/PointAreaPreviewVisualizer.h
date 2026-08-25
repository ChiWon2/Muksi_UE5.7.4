#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "PointAreaPreviewVisualizer.generated.h"

class ATargetingPreviewActor;
class UMaterialInterface;
class UStaticMesh;
struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UPointAreaPreviewVisualizer : public UAreaPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor) override;
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> PointPreviewMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> PointPreviewMaterial = nullptr;

	float PreviewHeightOffset = 5.0f;
	float PreviewMeshBaseSize = 100.0f;
};
