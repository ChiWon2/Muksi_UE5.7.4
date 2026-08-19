#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "StatusEffectExecution.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UStatusEffectExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;
};
