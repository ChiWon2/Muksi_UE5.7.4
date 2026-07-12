#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"
#include "MuksiBattleDamageExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiBattleDamageExecutionData : public FMuksiBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	int32 DamageValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	bool bTriggerHitReaction = true;
};

