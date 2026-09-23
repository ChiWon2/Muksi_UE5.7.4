#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StructUtils/InstancedStruct.h"
#include "DamageModifier.generated.h"

class ABattleCharacterBase;
struct FBattleExecutionContext;

UCLASS(Abstract)
class MUKSI_API UMuksiDamageModifier : public UObject
{
	GENERATED_BODY()

public:
	virtual int32 ModifyDamage(
		const FBattleExecutionContext& Context,
		ABattleCharacterBase* TargetCharacter,
		int32 Damage,
		const FInstancedStruct& ModifierData) const;

	virtual const UScriptStruct* GetModifierDataStruct() const;

protected:
	bool IsModifierDataValid(const FInstancedStruct& ModifierData) const;
};
