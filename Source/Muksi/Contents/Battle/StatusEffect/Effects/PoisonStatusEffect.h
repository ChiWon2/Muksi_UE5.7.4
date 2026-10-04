#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "PoisonStatusEffect.generated.h"

UCLASS()
class MUKSI_API UPoisonStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	UPoisonStatusEffect();

	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
};
