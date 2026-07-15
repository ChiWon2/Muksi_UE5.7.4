#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingPathPreviewStrategyBase.h"
#include "MuksiTargetingArcPathPreviewStrategy.generated.h"

UCLASS()
class MUKSI_API UMuksiTargetingArcPathPreviewStrategy : public UMuksiTargetingPathPreviewStrategyBase
{
	GENERATED_BODY()

public:
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings) override;
	virtual void Hide() override;

private:
	void EnsureArcPathMeshes(int32 RequiredSegmentCount);
	void HideArcPathMeshes();
	FVector GetQuadraticBezierPoint(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint, float Alpha) const;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> ArcPathMeshes;
};