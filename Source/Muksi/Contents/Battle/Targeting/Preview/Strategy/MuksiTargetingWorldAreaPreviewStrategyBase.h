#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiTargetingWorldAreaPreviewStrategyBase.generated.h"

class UStaticMeshComponent;
struct FMuksiTargetingPreviewCommand;
struct FMuksiTargetingWorldAreaPreviewSettings;

UCLASS(Abstract)
class MUKSI_API UMuksiTargetingWorldAreaPreviewStrategyBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(UStaticMeshComponent* InWorldAreaMesh);
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings);
	virtual void Hide();

protected:
	FVector GetPlaneScaleByRadius(float Radius) const;

protected:
	static constexpr float BasePlaneSize = 100.0f;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> WorldAreaMesh = nullptr;
};