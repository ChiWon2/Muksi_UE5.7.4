#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/DamageModifier.h"
#include "TargetHasStatusEffectDamageModifier.generated.h"

UCLASS()
class MUKSI_API UTargetHasStatusEffectDamageModifier : public UMuksiDamageModifier
{
	GENERATED_BODY()

public:
	virtual int32 ModifyDamage(
		const FBattleExecutionContext& Context,
		ABattleCharacterBase* TargetCharacter,
		int32 Damage,
		const FInstancedStruct& ModifierData) const override;

	virtual const UScriptStruct* GetModifierDataStruct() const override;
};
