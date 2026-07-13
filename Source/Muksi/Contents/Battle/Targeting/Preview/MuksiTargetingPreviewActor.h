#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"
#include "MuksiTargetingPreviewActor.generated.h"

class ABattleGridManager;
class USceneComponent;
class USplineComponent;
class USplineMeshComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class MUKSI_API AMuksiTargetingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AMuksiTargetingPreviewActor();

	void InitializePreviewActor(ABattleGridManager* InGridManager);
	void UpdatePreview(const FMuksiTargetingPreviewCommand& Command);
	void HidePreview();

	UFUNCTION(BlueprintPure, Category = "Battle|Preview")
	const FMuksiTargetingPreviewCommand& GetCurrentCommand() const { return CurrentCommand; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UpdateSelectionRangePreview();
	void UpdateWorldAreaPreview();
	void UpdatePathPreview();
	void UpdateStraightPathPreview();
	void HidePathPreview();
	void UpdateGridPreview();
	float GetSelectionRangeWorldRadius() const;
	FVector GetPlaneScaleByRadius(float Radius) const;
	TArray<FIntPoint> CombineCoords(const TArray<FIntPoint>& A, const TArray<FIntPoint>& B) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	// 시전자 주변 카드 선택 가능 사거리를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> RangePreviewMesh = nullptr;

	// 현재 조준 위치 주변의 실제 효과 범위를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> WorldAreaMesh = nullptr;

	// 직선 또는 화살표 형태의 경로를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> ArrowMesh = nullptr;

	// 직선 또는 곡선 형태의 경로를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<USplineComponent> PathSpline = nullptr;

	// Straight Path를 실제 화면에 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<USplineMeshComponent> StraightPathMesh = nullptr;

	// Preview Plane Mesh의 원본 한 변 길이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview", meta = (ClampMin = "1.0"))
	float BasePlaneSize = 100.0f;

	// Straight Path Mesh의 원본 단면 크기.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview", meta = (ClampMin = "1.0"))
	float BasePathMeshSize = 100.0f;

	// 사거리 Preview를 바닥보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview")
	float RangePreviewHeightOffset = 4.0f;

	// 효과 범위 Preview를 바닥보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview")
	float WorldAreaPreviewHeightOffset = 6.0f;

	// Straight Path를 바닥보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview")
	float PathPreviewHeightOffset = 8.0f;

	// Straight Path의 좌우 폭.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview", meta = (ClampMin = "1.0"))
	float StraightPathWidth = 35.0f;

	// Straight Path의 두께.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview", meta = (ClampMin = "0.1"))
	float StraightPathThickness = 2.0f;

	// 선택 가능 타일의 중심까지를 사거리 원으로 표시할지 결정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview")
	bool bIncludeOuterTileRadius = true;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	FMuksiTargetingPreviewCommand CurrentCommand;
};