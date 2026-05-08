#include"Cond_Quest_Register.h"
#include "../Cond_Quest.h"
#include"../../GameConditionRegistry.h"

// TODO: 실제 QuestSystem으로 교체
static bool IsQuestCompleted(UObject* WorldContext, FName QuestID)
{
	if (QuestID == "MainQuest")
		return true;

	return false;
}

bool CheckQuest(UObject* WorldContext, const FCond_Quest& Cond)
{
	bool bDone = IsQuestCompleted(WorldContext, Cond.QuestID);

	// true == 완료 필요
	// false == 미완료 필요
	return (bDone == Cond.bCompleted);
}

namespace ConditionRegister
{
	void Register_Cond_Quest()
	{
		FGameConditionRegistry::RegisterCondition<FCond_Quest>(CheckQuest);
	}
}