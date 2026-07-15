#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"
#include "MuksiTargetingPreviewActor.generated.h"

class ABattleGridManager;
class USceneComponent;
class USplineComponent;
class UStaticMeshComponent;
class UMuksiTargetingPreviewRenderer;
struct FMuksiTargetingPreviewSettings;

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
	FMuksiTargetingPreviewSettings MakePreviewSettings() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	// 시전자 주변 카드 선택 가능 사거리를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> RangePreviewMesh = nullptr;

	// 현재 조준 위치 주변의 실제 효과 범위를 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> WorldAreaMesh = nullptr;

	// 화살표 형태의 경로 끝부분을 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> ArrowMesh = nullptr;

	// 직선 또는 곡선 경로의 좌표를 관리한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<USplineComponent> PathSpline = nullptr;

	// Straight 또는 Arrow Path의 몸통을 표시한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Preview")
	TObjectPtr<UStaticMeshComponent> StraightPathMesh = nullptr;

	// 사거리 Preview를 바닥보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Selection Range")
	float RangePreviewHeightOffset = 4.0f;

	// 선택 가능 타일의 중심까지를 사거리 원으로 표시할지 결정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Selection Range")
	bool bIncludeOuterTileRadius = true;

	// 효과 범위 Preview를 바닥보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|World Area")
	float WorldAreaPreviewHeightOffset = 6.0f;

	// Path Preview를 시작점과 끝점보다 위에 표시할 높이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path")
	float PathPreviewHeightOffset = 8.0f;

	// Straight, Arrow, Arc 경로가 공통으로 사용할 월드 너비.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path", meta = (ClampMin = "1.0"))
	float PathWidth = 35.0f;

	// Arrow 화살촉의 월드 길이.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path", meta = (ClampMin = "1.0"))
	float ArrowHeadLength = 100.0f;

	// Arrow 화살촉의 월드 너비.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path", meta = (ClampMin = "1.0"))
	float ArrowHeadWidth = 100.0f;

	// Arc를 구성할 Plane Segment 개수.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path", meta = (ClampMin = "2", ClampMax = "64"))
	int32 ArcSegmentCount = 16;

	// Arc Segment 사이의 틈을 방지하기 위한 길이 배율.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Preview|Path", meta = (ClampMin = "1.0", ClampMax = "1.5"))
	float ArcSegmentLengthMultiplier = 1.05f;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiTargetingPreviewRenderer> PreviewRenderer = nullptr;

	UPROPERTY(Transient)
	FMuksiTargetingPreviewCommand CurrentCommand;
};