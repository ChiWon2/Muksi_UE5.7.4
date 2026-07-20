#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"

#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"

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

bool FBattleNotifyExecutionChain::IsValid() const
{
	return !NotifyKey.IsNone() && !Executions.IsEmpty();
}

void FBattleNotifyExecutionChain::SyncExecutionDataTypes()
{
	for (FBattleExecutionEntry& Entry : Executions)
	{
		Entry.SyncExecutionDataType();
	}
}