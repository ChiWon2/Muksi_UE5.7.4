#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "BattleExecution.generated.h"

DECLARE_DELEGATE(FBattleExecutionFinished);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UBattleExecution : public UObject
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished);
	virtual const UScriptStruct* GetExecutionDataStruct() const;
	bool IsPresentationOnly() const { return bPresentationOnly; }

protected:
	// Class-level semantic flag. Keep this C++-only so execution authors decide
	// whether an execution exists solely for visible presentation.
	// DD/DA simulation worlds skip these entries, while AD/AA execute them normally.
	bool bPresentationOnly = false;

	void FinishExecution(FBattleExecutionFinished& OnFinished);
	bool IsExecutionFinished() const { return bExecutionFinished; }

private:
	bool bExecutionFinished = false;
};