#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"

#include "StraightPathPreviewVisualizer.generated.h"

class ATargetingPreviewActor;
class UMaterialInterface;
class UStaticMesh;
struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UStraightPathPreviewVisualizer : public UPathPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor) override;
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;
	virtual const UScriptStruct* GetPathPreviewDataStruct() const override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> StraightPreviewMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> StraightPreviewMaterial = nullptr;

	float PreviewHeightOffset = 5.0f;
	float PreviewLineThickness = 10.0f;
	float PreviewMeshBaseSize = 100.0f;
};