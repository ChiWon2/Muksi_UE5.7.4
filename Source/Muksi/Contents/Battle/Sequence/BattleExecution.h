#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionContext.h"
#include "BattleExecution.generated.h"

DECLARE_DELEGATE(FBattleExecutionFinished);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UBattleExecution : public UObject
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished);
	virtual const UScriptStruct* GetExecutionDataStruct() const;

protected:
	void FinishExecution(FBattleExecutionFinished& OnFinished);
	bool IsExecutionFinished() const { return bExecutionFinished; }

private:
	bool bExecutionFinished = false;
};