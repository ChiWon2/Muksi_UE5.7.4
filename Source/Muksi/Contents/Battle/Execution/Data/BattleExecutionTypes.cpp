#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"

#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"

bool FBattleExecutionEntry::IsValid() const
{
	return ExecutionClass != nullptr;
}

void FBattleExecutionEntry::SyncExecutionDataType()
{
	if (!ExecutionClass)
	{
		ExecutionData.Reset();
		return;
	}

	const UBattleExecution* ExecutionCDO = ExecutionClass.GetDefaultObject();

	if (!ExecutionCDO)
	{
		ExecutionData.Reset();
		return;
	}

	const UScriptStruct* ExpectedStruct = ExecutionCDO->GetExecutionDataStruct();

	if (!ExpectedStruct)
	{
		ExecutionData.Reset();
		return;
	}

	if (ExecutionData.GetScriptStruct() == ExpectedStruct)
	{
		return;
	}

	ExecutionData.InitializeAs(ExpectedStruct);
}

bool FBattleExecutionNotify::IsValid() const
{
	return !NotifyKey.IsNone() && !ExecutionEntries.IsEmpty();
}

void FBattleExecutionNotify::SyncExecutionDataTypes()
{
	for (FBattleExecutionEntry& Entry : ExecutionEntries)
	{
		Entry.SyncExecutionDataType();
	}
}