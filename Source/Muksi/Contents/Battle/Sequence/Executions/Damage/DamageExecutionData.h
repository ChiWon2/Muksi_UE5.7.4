#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "DamageExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FDamageExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	int32 DamageValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	bool bTriggerHitReaction = true;
};