#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "DamageModifierData.generated.h"

class UMuksiDamageModifier;

USTRUCT(BlueprintType)
struct FDamageModifierData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FDamageModifierEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	TSubclassOf<UMuksiDamageModifier> ModifierClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier", meta = (DisplayName = "Data", BaseStruct = "/Script/Muksi.DamageModifierData", EditCondition = "ModifierClass != nullptr", EditConditionHides))
	FInstancedStruct ModifierData;
};
