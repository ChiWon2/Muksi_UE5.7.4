#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

#include "ArcPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FArcPathPreviewData : public FPathPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arc Path Preview", meta = (ClampMin = "0.0"))
	float ArcHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arc Path Preview", meta = (ClampMin = "2"))
	int32 SegmentCount = 16;
};