#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"

#include "CircleRangePreviewVisualizer.generated.h"

class ATargetingPreviewActor;
class UMaterialInterface;
class UStaticMesh;
struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UCircleRangePreviewVisualizer : public USelectionPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor) override;
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;

private:
	float CalculateWorldRadius(const FTargetingPreviewContext& Context) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CirclePreviewMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> RangePreviewMaterial = nullptr;

	float PreviewHeightOffset = 5.0f;
	float PreviewMeshBaseSize = 100.0f;
};