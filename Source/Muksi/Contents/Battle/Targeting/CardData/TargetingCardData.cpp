#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"

#if WITH_EDITOR

void FTargetingCardData::SyncDataTypes()
{
	SyncStepDataTypes();
}

void FTargetingCardData::SyncStepDataTypes()
{
	for (FTargetingStepCardData& Step : Steps)
	{
		Step.SyncDataTypes();
	}
}

#endif