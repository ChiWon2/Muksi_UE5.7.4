#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "SetGlobalTimeDilationExecution.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API USetGlobalTimeDilationExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	USetGlobalTimeDilationExecution();
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;
};
