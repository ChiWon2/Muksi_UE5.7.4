#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/DamageModifier.h"
#include "TargetHasStatusEffectDamageModifier.generated.h"

UCLASS(EditInlineNew, DefaultToInstanced)
class MUKSI_API UTargetHasStatusEffectDamageModifier : public UMuksiDamageModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	FName StatusEffectID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	int32 AdditionalDamage = 0;

	virtual int32 ModifyDamage(
		const FBattleExecutionContext& Context,
		ABattleCharacterBase* TargetCharacter,
		int32 Damage) const override;
};
