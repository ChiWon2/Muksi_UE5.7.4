#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiTargetingSelectionRangePreviewRenderer.generated.h"

class ABattleGridManager;
class UStaticMeshComponent;
struct FMuksiTargetingPreviewCommand;

struct FMuksiTargetingSelectionRangePreviewSettings
{
	float HeightOffset = 4.0f;
	bool bIncludeOuterTileRadius = true;
};

UCLASS()
class MUKSI_API UMuksiTargetingSelectionRangePreviewRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UStaticMeshComponent* InRangePreviewMesh);
	void SetGridManager(ABattleGridManager* InGridManager);
	void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingSelectionRangePreviewSettings& Settings);
	void Hide();

private:
	float GetSelectionRangeWorldRadius(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingSelectionRangePreviewSettings& Settings) const;
	FVector GetPlaneScaleByRadius(float Radius) const;

private:
	static constexpr float BasePlaneSize = 100.0f;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> RangePreviewMesh = nullptr;
};