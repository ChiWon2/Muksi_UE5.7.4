#include"Cond_Level_Register.h"
#include "../Cond_Level.h"
#include "ConditionRegisterUtils.h"
#include"../../GameConditionRegistry.h"


// TODO: 실제 PlayerStatSystem으로 교체
static int32 GetLevel(UObject* WorldContext, EConditionTarget Target)
{
	switch (Target)
	{
	case EConditionTarget::Self:
		return 10;

	case EConditionTarget::Target:
		return 5;
	}

	return 0;
}

bool CheckLevel(UObject* WorldContext, const FCond_Level& Cond)
{
	int32 Level = GetLevel(WorldContext, Cond.Target);
	return Compare<int32>(Level, Cond.RequiredLevel, Cond.CompareOp);
}

namespace ConditionRegister
{
	void Register_Cond_Level()
	{
		FGameConditionRegistry::RegisterCondition<FCond_Level>(CheckLevel);
	}
}