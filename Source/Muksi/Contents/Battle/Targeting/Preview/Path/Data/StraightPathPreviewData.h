#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

#include "StraightPathPreviewData.generated.h"

USTRUCT(BlueprintType)
struct FStraightPathPreviewData : public FPathPreviewData
{
	GENERATED_BODY()

	// Origin에서 표시하는 직선 경로의 길이.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Straight Path Preview", meta = (ClampMin = "0.0"))
	float Length = 500.0f;

	// Aim 거리와 관계없이 설정된 Length를 사용한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Straight Path Preview")
	bool bUseFixedLength = true;
};