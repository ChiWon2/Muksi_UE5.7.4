#include "RegisterAllCondition.h"
#include"Cond_Dialogue_Register.h"
#include"Cond_Equip_Register.h"
#include"Cond_Level_Register.h"
#include"Cond_HasItem_Register.h"
#include"Cond_Quest_Register.h"
#include"Cond_Stat_Register.h"
#include"Cond_TravelTime_Register.h"

namespace ConditionRegister
{
	void RegisterAllConditions()
	{
		Register_Cond_Dialogue();
		Register_Cond_Equip();
		Register_Cond_Level();
		Register_Cond_HasItem();
		Register_Cond_Quest();
		Register_Cond_Stat();
		Register_Cond_TravelTime();
	}
}