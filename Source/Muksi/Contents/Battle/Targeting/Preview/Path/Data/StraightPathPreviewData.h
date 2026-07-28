#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

#include "StraightPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FStraightPathPreviewData : public FPathPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Straight Path Preview", meta = (ClampMin = "0.0"))
	float Length = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Straight Path Preview")
	bool bUseFixedLength = true;
};