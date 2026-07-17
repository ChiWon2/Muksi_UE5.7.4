#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "CircleAreaPreviewVisualizer.generated.h"

class ATargetingPreviewActor;
class UMaterialInterface;
class UStaticMesh;
struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UCircleAreaPreviewVisualizer : public UAreaPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor) override;
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const override;

private:
	float CalculateWorldRadius(const FTargetingPreviewContext& Context, const FVector& CenterLocation) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CirclePreviewMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> CirclePreviewMaterial = nullptr;

	float PreviewHeightOffset = 5.0f;
	float PreviewMeshBaseSize = 100.0f;
};