#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "BleedStatusEffect.generated.h"

UCLASS()
class MUKSI_API UBleedStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	UBleedStatusEffect();

	virtual void EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction) override;
	
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
};
