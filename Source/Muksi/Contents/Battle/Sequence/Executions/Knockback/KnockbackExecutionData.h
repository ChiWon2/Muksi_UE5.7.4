#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "KnockbackExecutionData.generated.h"


USTRUCT(BlueprintType)
struct FKnockbackExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	// 피격자를 밀어낼 최대 Grid 거리.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback", meta = (ClampMin = "0"))
	int32 Range = 1;

	// 피격자의 초당 이동 속도.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback", meta = (ClampMin = "0.0"))
	float MoveSpeed = 1000.0f;
};