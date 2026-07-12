#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"

#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"

void FMuksiBattleExecutionEntry::SyncExecutionDataType()
{
	if (!ExecutionClass)
	{
		ExecutionData.Reset();
		return;
	}

	const UMuksiBattleExecution* ExecutionCDO = ExecutionClass.GetDefaultObject();

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

void FMuksiBattleNotifyExecutionBinding::SyncExecutionDataTypes()
{
	for (FMuksiBattleExecutionEntry& Entry : ExecutionChain)
	{
		Entry.SyncExecutionDataType();
	}
}
