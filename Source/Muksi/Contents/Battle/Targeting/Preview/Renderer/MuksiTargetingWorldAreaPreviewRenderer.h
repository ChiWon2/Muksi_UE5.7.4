#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiTargetingWorldAreaPreviewRenderer.generated.h"

class UStaticMeshComponent;
struct FMuksiTargetingPreviewCommand;

struct FMuksiTargetingWorldAreaPreviewSettings
{
	float HeightOffset = 6.0f;
};

UCLASS()
class MUKSI_API UMuksiTargetingWorldAreaPreviewRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UStaticMeshComponent* InWorldAreaMesh);
	void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings);
	void Hide();

private:
	void UpdateCirclePreview(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings);
	FVector GetPlaneScaleByRadius(float Radius) const;

private:
	static constexpr float BasePlaneSize = 100.0f;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> WorldAreaMesh = nullptr;
};