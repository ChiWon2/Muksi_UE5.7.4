#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "StopMontageExecution.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UStopMontageExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	UStopMontageExecution();
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;
};
