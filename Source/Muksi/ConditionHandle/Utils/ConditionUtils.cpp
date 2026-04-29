#include "ConditionUtils.h"

namespace ConditionUtils
{
	//////////TravelTime//////////////
	FInstancedStruct MakeTravelTime(FTravelDate Date, EConditionCompareOp Op)
	{
		FCond_TravelTime Cond;
		Cond.Date = Date;
		Cond.CompareOp = Op;

		return FInstancedStruct::Make(Cond);
	}

	bool CheckTravelTime(UObject* WorldContext, FTravelDate Date, EConditionCompareOp Op)
	{
		return FGameConditionEvaluator::Check(
			WorldContext,
			MakeTravelTime(Date, Op)
		);
	}
}