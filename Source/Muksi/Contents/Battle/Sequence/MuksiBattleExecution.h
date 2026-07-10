#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiBattleExecutionContext.h"
#include "MuksiBattleExecution.generated.h"

DECLARE_DELEGATE(FMuksiBattleExecutionFinished);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleExecution : public UObject
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished);

protected:
	void FinishExecution(FMuksiBattleExecutionFinished& OnFinished) const;
};