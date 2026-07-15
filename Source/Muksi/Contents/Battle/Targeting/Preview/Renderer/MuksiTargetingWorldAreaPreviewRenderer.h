#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiTargetingWorldAreaPreviewRenderer.generated.h"

class UStaticMeshComponent;
class UMuksiTargetingCircleWorldAreaPreviewStrategy;
class UMuksiTargetingConeWorldAreaPreviewStrategy;
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
	void Initialize(UStaticMeshComponent* InCircleWorldAreaMesh, UStaticMeshComponent* InConeWorldAreaMesh);
	void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings);
	void Hide();

private:
	void HideAllStrategies();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingCircleWorldAreaPreviewStrategy> CircleStrategy = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingConeWorldAreaPreviewStrategy> ConeStrategy = nullptr;
};