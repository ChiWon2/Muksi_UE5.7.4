#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "SetCurrentMontagePlayRateExecution.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API USetCurrentMontagePlayRateExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	USetCurrentMontagePlayRateExecution();
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;
};
