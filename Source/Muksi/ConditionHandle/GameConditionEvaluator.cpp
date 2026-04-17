#include "GameConditionEvaluator.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

// 나중에 연결
// #include "QuestSubsystem.h"
// #include "InventorySubsystem.h"
// #include "PlayerSubsystem.h"

bool FGameConditionEvaluator::Check(UObject* WorldContext, const FGameCondition& Condition)
{
	bool bResult = false;

	switch (Condition.Type)
	{
	case EGameConditionType::QuestCompleted:
		bResult = CheckQuestCompleted(WorldContext, Condition);
		break;

	case EGameConditionType::CharacterLevel:
		bResult = CheckCharacterLevel(WorldContext, Condition);
		break;

	case EGameConditionType::HasItem:
		bResult = CheckHasItem(WorldContext, Condition);
		break;

	default:
		bResult = false;
		break;
	}

	// NOT 조건 처리
	return Condition.bInvert ? !bResult : bResult;
}

bool FGameConditionEvaluator::CheckAll(UObject* WorldContext, const TArray<FGameCondition>& Conditions)
{
	for (const FGameCondition& Cond : Conditions)
	{
		if (!Check(WorldContext, Cond))
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------
// 개별 조건
//--------------------------------------------------

bool FGameConditionEvaluator::CheckQuestCompleted(UObject* WorldContext, const FGameCondition& Condition)
{
	// UQuestSubsystem* Quest = GI->GetSubsystem<UQuestSubsystem>();
	// return Quest && Quest->IsQuestCompleted(Condition.NameValue);

	return false;
}

bool FGameConditionEvaluator::CheckCharacterLevel(UObject* WorldContext, const FGameCondition& Condition)
{
	// 예시:
	// return PlayerLevel >= Condition.IntValue;

	return false;
}

bool FGameConditionEvaluator::CheckHasItem(UObject* WorldContext, const FGameCondition& Condition)
{
	// 예시:
	// return Inventory->HasItem(Condition.NameValue, Condition.IntValue);

	return false;
}