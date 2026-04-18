#include"Cond_Equip_Register.h"
#include "../Cond_Equip.h"
#include"../../GameConditionRegistry.h"

// TODO: 실제 EquipmentSystem으로 교체
static bool IsEquipped(UObject* WorldContext, EConditionTarget Target, FName ItemID)
{
	// 예시 로직

	if (Target == EConditionTarget::Self)
	{
		if (ItemID == "Sword")
		{
			return true;
		}
	}

	if (Target == EConditionTarget::Target)
	{
		if (ItemID == "Shield")
		{
			return true;
		}
	}

	return false;
}

// 실제 조건 체크
static bool CheckEquip(UObject* WorldContext, const FCond_Equip& Cond)
{
	return IsEquipped(WorldContext, Cond.Target, Cond.ItemID);
}

namespace ConditionRegister
{
	void Register_Cond_Equip()
	{
		FGameConditionRegistry::RegisterCondition<FCond_Equip>(CheckEquip);
	}
}