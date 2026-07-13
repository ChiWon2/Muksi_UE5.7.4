#pragma once

#include "CoreMinimal.h"
#include "MuksiCardTargetingData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiCardTargetingData
{
	GENERATED_BODY()

	// 시전자 위치를 기준으로 선택 가능한 최대 그리드 거리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ClampMin = "0"))
	int32 SelectionRange = 1;
};