#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionData.h"

#include "DirectionSelectionData.generated.h"

USTRUCT(BlueprintType)
struct FDirectionSelectionData : public FTargetSelectionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction Selection", meta = (ClampMin = "0.0"))
	float MinimumAimDistance = 1.0f;
};