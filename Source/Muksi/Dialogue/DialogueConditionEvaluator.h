#pragma once

#include "CoreMinimal.h"
#include "DialogueCondition.h"

class FDialogueConditionEvaluator
{
public:
	// 단일 조건
	static bool CheckCondition(UObject* WorldContext, const FDialogueCondition& Condition);

	// 여러 조건 (AND)
	static bool CheckConditions(UObject* WorldContext, const TArray<FDialogueCondition>& Conditions);

private:
	// 개별 조건 함수들
	static bool CheckQuestCompleted(UObject* WorldContext, const FDialogueCondition& Condition);

};