#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatModifierStatusEffect.h"
#include "HasteStatusEffect.generated.h"

UCLASS()
class MUKSI_API UHasteStatusEffect : public UMuksiStatModifierStatusEffect
{
	GENERATED_BODY()

public:
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;

protected:
	virtual void ApplyStatModifier(int32 Amount) override;
};
