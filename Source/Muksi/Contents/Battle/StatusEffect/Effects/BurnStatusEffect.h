#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "BurnStatusEffect.generated.h"

UCLASS()
class MUKSI_API UBurnStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	UBurnStatusEffect();

	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
};
