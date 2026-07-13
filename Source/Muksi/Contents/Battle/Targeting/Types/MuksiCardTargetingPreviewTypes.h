#pragma once

#include "CoreMinimal.h"
#include "MuksiCardTargetingPreviewTypes.generated.h"

UENUM(BlueprintType)
enum class EMuksiCardTargetingPathPreviewStyle : uint8
{
	None UMETA(DisplayName = "None"),
	Straight UMETA(DisplayName = "Straight"),
	Arc UMETA(DisplayName = "Arc"),
	Arrow UMETA(DisplayName = "Arrow")
};

UENUM(BlueprintType)
enum class EMuksiCardTargetingWorldAreaPreviewStyle : uint8
{
	None UMETA(DisplayName = "None"),
	Circle UMETA(DisplayName = "Circle"),
	Cone UMETA(DisplayName = "Cone"),
	Rectangle UMETA(DisplayName = "Rectangle")
};

UENUM(BlueprintType)
enum class EMuksiCardTargetingGridPreviewStyle : uint8
{
	None UMETA(DisplayName = "None"),
	AffectedTiles UMETA(DisplayName = "Affected Tiles"),
	PathTiles UMETA(DisplayName = "Path Tiles"),
	AffectedAndPathTiles UMETA(DisplayName = "Affected And Path Tiles")
};

USTRUCT(BlueprintType)
struct FMuksiCardTargetingPreviewData
{
	GENERATED_BODY()

	// 시전자 주변에 카드 선택 가능 사거리를 표시할지 결정한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	bool bShowSelectionRange = true;

	// 시전자에서 조준 위치까지의 경로 표시 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	EMuksiCardTargetingPathPreviewStyle PathStyle = EMuksiCardTargetingPathPreviewStyle::None;

	// 마우스 월드 위치를 따라가는 범위 표시 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	EMuksiCardTargetingWorldAreaPreviewStyle WorldAreaStyle = EMuksiCardTargetingWorldAreaPreviewStyle::None;

	// 실제 적용될 그리드 좌표의 표시 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	EMuksiCardTargetingGridPreviewStyle GridStyle = EMuksiCardTargetingGridPreviewStyle::AffectedTiles;

	// 곡선 경로를 표시할 때 사용하는 최대 높이.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview", meta = (EditCondition = "PathStyle == EMuksiCardTargetingPathPreviewStyle::Arc", EditConditionHides, ClampMin = "0.0"))
	float ArcHeight = 1000.0f;

	// 월드 범위 표시의 반지름 또는 대표 크기.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview", meta = (EditCondition = "WorldAreaStyle != EMuksiCardTargetingWorldAreaPreviewStyle::None", EditConditionHides, ClampMin = "0.0"))
	float WorldAreaRadius = 100.0f;

	// 원뿔형 월드 범위 표시의 각도.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview", meta = (EditCondition = "WorldAreaStyle == EMuksiCardTargetingWorldAreaPreviewStyle::Cone", EditConditionHides, ClampMin = "0.0", ClampMax = "360.0"))
	float WorldAreaAngle = 60.0f;

	// 사각형 월드 범위 표시의 크기.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview", meta = (EditCondition = "WorldAreaStyle == EMuksiCardTargetingWorldAreaPreviewStyle::Rectangle", EditConditionHides))
	FVector2D WorldAreaSize = FVector2D(200.0f, 200.0f);
};