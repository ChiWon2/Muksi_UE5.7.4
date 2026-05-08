#include"Cond_TravelTime_Register.h"
#include "../Cond_TravelTime.h"
#include "ConditionRegisterUtils.h"
#include"../../GameConditionRegistry.h"
#include"../TravelTime/TravelTimeSubsystem.h"

bool CheckTravelTime(UObject* WorldContext, const FCond_TravelTime& Cond)
{
	if (!WorldContext) return false;

	const UTravelTimeSubsystem* Sys = UTravelTimeSubsystem::Get(WorldContext);
	if (!Sys) return false;

	const int64 Current = FTravelTimeUtils::DateToHours(Sys->GetCurrentDate());
	const int64 Target = FTravelTimeUtils::DateToHours(Cond.Date);

	return Compare<int64>(Current, Target, Cond.CompareOp);
}

namespace ConditionRegister
{
	void Register_Cond_TravelTime()
	{
		FGameConditionRegistry::RegisterCondition<FCond_TravelTime>(CheckTravelTime);
	}
}