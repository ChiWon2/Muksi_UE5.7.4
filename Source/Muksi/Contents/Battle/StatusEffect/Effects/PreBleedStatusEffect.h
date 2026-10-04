#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "PreBleedStatusEffect.generated.h"

UCLASS()
class MUKSI_API UPreBleedStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	UPreBleedStatusEffect();

	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
};
