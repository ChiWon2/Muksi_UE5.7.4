#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/StraightPathPreviewData.h"

#include "ArrowPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FArrowPathPreviewData : public FStraightPathPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arrow Path Preview", meta = (ClampMin = "0.0"))
	float ArrowHeadLength = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arrow Path Preview", meta = (ClampMin = "0.0"))
	float ArrowHeadWidth = 100.0f;
};