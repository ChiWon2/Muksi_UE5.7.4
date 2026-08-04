#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Base/Cond_Targeting_Base.h"
#include "Cond_TargetingOccupancy.generated.h"

UENUM(BlueprintType)
enum class ETargetingOccupancyPolicy : uint8
{
    Ignore,
    RequireEmpty,
    RequireOccupied,
    RequireEnemy,
    RequireAlly
};

/** 평가 좌표의 점유 상태 또는 점유 캐릭터와 Source의 관계를 검사한다. */
USTRUCT(BlueprintType, meta=(DisplayName="Targeting | Tile | Occupancy"))
struct FCond_TargetingOccupancy : public FCond_Targeting_Base
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    ETargetingOccupancyPolicy Policy = ETargetingOccupancyPolicy::RequireEmpty;

    static bool Evaluate(UObject* WorldContext, const FCond_TargetingOccupancy& Condition);
};
