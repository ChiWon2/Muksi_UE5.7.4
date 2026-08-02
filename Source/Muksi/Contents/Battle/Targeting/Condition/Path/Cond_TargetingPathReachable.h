#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Base/Cond_Targeting_Base.h"
#include "Cond_TargetingPathReachable.generated.h"

/** Origin에서 평가 좌표까지 이동 가능한 경로가 존재하는지 검사한다. */
USTRUCT(BlueprintType, meta=(DisplayName="Targeting | Path | Reachable"))
struct FCond_TargetingPathReachable : public FCond_Targeting_Base
{
    GENERATED_BODY()

    /** 예측 이동 선택처럼 현재 점유된 목적지도 목표 좌표로 허용한다. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    bool bAllowOccupiedGoal = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition", meta=(ClampMin="-1"))
    int32 MaxDistance = -1;

    static bool Evaluate(UObject* WorldContext, const FCond_TargetingPathReachable& Condition);
};
