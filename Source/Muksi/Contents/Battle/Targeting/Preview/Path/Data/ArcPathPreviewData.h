#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

#include "ArcPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FArcPathPreviewData : public FPathPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arc Path Preview", meta = (ClampMin = "0.0"))
	float Height = 300.0f;
};