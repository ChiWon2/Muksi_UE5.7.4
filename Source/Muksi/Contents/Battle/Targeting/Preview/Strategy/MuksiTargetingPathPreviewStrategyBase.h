#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"
#include "MuksiTargetingPathPreviewStrategyBase.generated.h"

class AActor;
class USceneComponent;
class USplineComponent;
class UStaticMeshComponent;

struct FMuksiTargetingPathPreviewSettings
{
	float PathPreviewHeightOffset = 8.0f;
	float PathWidth = 35.0f;
	float ArrowHeadLength = 100.0f;
	float ArrowHeadWidth = 100.0f;
	int32 ArcSegmentCount = 16;
	float ArcSegmentLengthMultiplier = 1.05f;
};

UCLASS(Abstract)
class MUKSI_API UMuksiTargetingPathPreviewStrategyBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline);
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings);
	virtual void Hide();

protected:
	void ApplyHeightOffset(FVector& StartWorldLocation, FVector& EndWorldLocation, float HeightOffset) const;
	void UpdatePathSegment(UStaticMeshComponent* SegmentMesh, const FVector& StartWorldLocation, const FVector& EndWorldLocation, float PathWidth, float LengthMultiplier = 1.0f) const;

protected:
	static constexpr float BasePathMeshLength = 100.0f;
	static constexpr float BasePathMeshWidth = 100.0f;
	static constexpr float BaseArrowMeshLength = 100.0f;
	static constexpr float BaseArrowMeshWidth = 100.0f;

	UPROPERTY(Transient)
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> StraightPathMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> ArrowMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> PathSpline = nullptr;
};