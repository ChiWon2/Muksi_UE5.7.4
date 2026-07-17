#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionData.h"

#include "DirectionSelectionData.generated.h"

USTRUCT(BlueprintType)
struct FDirectionSelectionData : public FTargetSelectionData
{
	GENERATED_BODY()

	// Origin과 Aim 사이에 필요한 최소 평면 거리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction Selection", meta = (ClampMin = "0.0"))
	float MinimumAimDistance = 1.0f;
};