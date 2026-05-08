#include"Cond_HasItem_Register.h"
#include "../Cond_HasItem.h"
#include"ConditionRegisterUtils.h"
#include"../../GameConditionRegistry.h"

// TODO: 실제 InventorySystem으로 교체
static int32 GetItemCount(UObject* WorldContext, FName ItemID)
{
	if (ItemID == "Potion")
		return 5;

	return 0;
}

bool CheckHasItem(UObject* WorldContext, const FCond_HasItem& Cond)
{
	int32 Count = GetItemCount(WorldContext, Cond.ItemID);
	return Compare<int32>(Count, Cond.Count, Cond.CompareOp);
}

namespace ConditionRegister
{
	void Register_Cond_HasItem()
	{
		FGameConditionRegistry::RegisterCondition<FCond_HasItem>(CheckHasItem);
	}
}