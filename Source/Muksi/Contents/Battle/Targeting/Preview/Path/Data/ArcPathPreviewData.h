#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

#include "ArcPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FArcPathPreviewData : public FPathPreviewData
{
	GENERATED_BODY()

	// 직선 경로 중앙에 추가하는 곡선 높이.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arc Path Preview", meta = (ClampMin = "0.0"))
	float ArcHeight = 300.0f;

	// 곡선 경로를 구성하는 Segment 개수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arc Path Preview", meta = (ClampMin = "2"))
	int32 SegmentCount = 16;
};