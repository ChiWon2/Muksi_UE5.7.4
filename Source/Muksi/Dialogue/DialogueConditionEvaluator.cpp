#include "DialogueConditionEvaluator.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
//#include "QuestSubsystem.h" // 나중에 연결

//--------------------------------------------------
// Public
//--------------------------------------------------

bool FDialogueConditionEvaluator::CheckCondition(UObject* WorldContext, const FDialogueCondition& Condition)
{
	switch (Condition.Type)
	{
	case EDialogueConditionType::QuestCompleted:
		return CheckQuestCompleted(WorldContext, Condition);
	case EDialogueConditionType::DateAfter:
		return CheckQuestCompleted(WorldContext, Condition);
	case EDialogueConditionType::DateBefore:
		return CheckQuestCompleted(WorldContext, Condition);
	case EDialogueConditionType::DialogueOptionSelected:
		return CheckQuestCompleted(WorldContext, Condition);

	default:
		return false;
	}
}

bool FDialogueConditionEvaluator::CheckConditions(UObject* WorldContext, const TArray<FDialogueCondition>& Conditions)
{
	for (const FDialogueCondition& Cond : Conditions)
	{
		if (!CheckCondition(WorldContext, Cond))
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------
// Private
//--------------------------------------------------

bool FDialogueConditionEvaluator::CheckQuestCompleted(UObject* WorldContext, const FDialogueCondition& Condition)
{
	// UQuestSubsystem* QuestSubSys = UQuestSubsystem::Get();
	// if (!QuestSubSys) return false;
	// return QuestSubSys->IsQuestCompleted(Condition.NameValue);

	return false;
}