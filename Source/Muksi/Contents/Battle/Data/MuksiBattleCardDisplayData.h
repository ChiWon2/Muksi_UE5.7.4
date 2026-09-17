#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardType.h"
#include "MuksiBattleCardDisplayData.generated.h"

USTRUCT(BlueprintType)
struct FCardEffectDisplayData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMuksiAttackCardType AttackType = EMuksiAttackCardType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AttackValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText EffectDescription;
};