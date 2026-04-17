#pragma once

#include "CoreMinimal.h"
#include "GameCondition.h"

class FGameConditionEvaluator
{
public:
	// 단일 조건
	static bool Check(UObject* WorldContext, const FGameCondition& Condition);

	// 여러 조건 (AND)
	static bool CheckAll(UObject* WorldContext, const TArray<FGameCondition>& Conditions);

private:
	// 개별 조건 처리
	static bool CheckQuestCompleted(UObject* WorldContext, const FGameCondition& Condition);
	static bool CheckCharacterLevel(UObject* WorldContext, const FGameCondition& Condition);
	static bool CheckHasItem(UObject* WorldContext, const FGameCondition& Condition);

};