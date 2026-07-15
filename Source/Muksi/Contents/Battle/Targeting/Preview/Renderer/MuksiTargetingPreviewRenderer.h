#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingPathPreviewRenderer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingSelectionRangePreviewRenderer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingWorldAreaPreviewRenderer.h"
#include "MuksiTargetingPreviewRenderer.generated.h"

class AActor;
class ABattleGridManager;
class USceneComponent;
class USplineComponent;
class UStaticMeshComponent;
class UMuksiTargetingGridPreviewRenderer;
struct FMuksiTargetingPreviewCommand;

struct FMuksiTargetingPreviewSettings
{
	FMuksiTargetingSelectionRangePreviewSettings SelectionRangeSettings;
	FMuksiTargetingWorldAreaPreviewSettings WorldAreaSettings;
	FMuksiTargetingPathPreviewSettings PathSettings;
};

UCLASS()
class MUKSI_API UMuksiTargetingPreviewRenderer : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, ABattleGridManager* InGridManager, UStaticMeshComponent* InRangePreviewMesh, UStaticMeshComponent* InWorldAreaMesh, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline);
	void SetGridManager(ABattleGridManager* InGridManager);
	void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPreviewSettings& Settings);
	void Hide();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingSelectionRangePreviewRenderer> SelectionRangeRenderer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingWorldAreaPreviewRenderer> WorldAreaRenderer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingGridPreviewRenderer> GridRenderer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingPathPreviewRenderer> PathRenderer = nullptr;
};