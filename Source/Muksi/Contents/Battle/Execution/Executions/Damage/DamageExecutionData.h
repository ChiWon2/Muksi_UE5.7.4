#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "DamageExecutionData.generated.h"

UENUM(BlueprintType)
enum class EDamageDefensePolicy : uint8
{
	ApplyDefense,
	IgnoreDefense
};

USTRUCT(BlueprintType)
struct FDamageExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EBattleExecutionTargetPolicy TargetPolicy = EBattleExecutionTargetPolicy::TargetingResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	int32 DamageValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EDamageDefensePolicy DefensePolicy = EDamageDefensePolicy::ApplyDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	bool bTriggerHitReaction = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	bool bTriggerStatusEffectReactions = true;
};
