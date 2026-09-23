#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/DamageModifierData.h"
#include "TargetHasStatusEffectDamageModifierData.generated.h"

USTRUCT(BlueprintType)
struct FTargetHasStatusEffectDamageModifierData : public FDamageModifierData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	FName StatusEffectID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	int32 AdditionalDamage = 0;
};
