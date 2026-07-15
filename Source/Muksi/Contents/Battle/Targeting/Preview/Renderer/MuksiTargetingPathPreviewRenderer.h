#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingPathPreviewStrategyBase.h"
#include "MuksiTargetingPathPreviewRenderer.generated.h"

class AActor;
class USceneComponent;
class USplineComponent;
class UStaticMeshComponent;
class UMuksiTargetingStraightPathPreviewStrategy;
class UMuksiTargetingArrowPathPreviewStrategy;
class UMuksiTargetingArcPathPreviewStrategy;

UCLASS()
class MUKSI_API UMuksiTargetingPathPreviewRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline);
	void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings);
	void Hide();

private:
	void HideAllStrategies();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingStraightPathPreviewStrategy> StraightStrategy = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingArrowPathPreviewStrategy> ArrowStrategy = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingArcPathPreviewStrategy> ArcStrategy = nullptr;
};