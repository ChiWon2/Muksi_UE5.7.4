#include"Cond_Dialogue_Register.h"
#include "../Cond_Dialogue.h"
#include"../../GameConditionRegistry.h"


// TODO: 실제 DialogueSystem으로 교체
static bool IsDialogueDone(UObject* WorldContext, FName DialogueID)
{
	if (DialogueID == "Intro")
		return true;

	return false;
}

bool CheckDialogue(UObject* WorldContext, const FCond_Dialogue& Cond)
{
	bool bDone = IsDialogueDone(WorldContext, Cond.DialogueID);
	return (bDone == Cond.bChecked);
}

namespace ConditionRegister
{
	void Register_Cond_Dialogue()
	{
		FGameConditionRegistry::RegisterCondition<FCond_Dialogue>(CheckDialogue);
	}
}