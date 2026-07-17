#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#if WITH_EDITOR

void FTargetingCardData::SyncDataTypes()
{
	SyncStepDataTypes();
	SyncPatternDataType();
}

void FTargetingCardData::SyncStepDataTypes()
{
	for (FTargetingStepCardData& Step : Steps)
	{
		Step.SyncDataTypes();
	}
}

void FTargetingCardData::SyncPatternDataType()
{
	if (!FinalPatternClass)
	{
		FinalPatternData.Reset();
		return;
	}

	const UAreaPattern* PatternCDO = FinalPatternClass.GetDefaultObject();

	if (!PatternCDO)
	{
		FinalPatternData.Reset();
		return;
	}

	const UScriptStruct* ExpectedDataStruct = PatternCDO->GetPatternDataStruct();

	if (!ExpectedDataStruct)
	{
		FinalPatternData.Reset();
		return;
	}

	if (FinalPatternData.GetScriptStruct() == ExpectedDataStruct)
	{
		return;
	}

	FinalPatternData.InitializeAs(ExpectedDataStruct);
}

#endif