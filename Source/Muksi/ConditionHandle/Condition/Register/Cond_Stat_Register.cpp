#include"Cond_Stat_Register.h"
#include "../Cond_Stat.h"
#include"../../GameConditionRegistry.h"
#include "ConditionRegisterUtils.h"

// TODO: 실제 StatSystem으로 교체
static int32 GetStat(UObject* WorldContext, EConditionTarget Target, FName StatName)
{
	// 예시
	if (StatName == "HP")
	{
		return (Target == EConditionTarget::Self) ? 100 : 50;
	}

	if (StatName == "Attack")
	{
		return (Target == EConditionTarget::Self) ? 20 : 10;
	}

	return 0;
}

bool CheckStat(UObject* WorldContext, const FCond_Stat& Cond)
{
	int32 CurrentValue = GetStat(WorldContext, Cond.Target, Cond.StatName);

	return Compare<int32>(CurrentValue, Cond.Value, Cond.CompareOp);
}

namespace ConditionRegister
{
	void Register_Cond_Stat()
	{
		FGameConditionRegistry::RegisterCondition<FCond_Stat>(CheckStat);
	}
}