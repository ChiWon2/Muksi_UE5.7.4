#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "ParalysisStatusEffect.generated.h"

UCLASS()
class MUKSI_API UParalysisStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	UParalysisStatusEffect();

	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
};
