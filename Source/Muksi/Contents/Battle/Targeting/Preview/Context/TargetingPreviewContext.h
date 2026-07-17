#pragma once

#include "CoreMinimal.h"

class ABattleCharacterBase;
class ABattleGridManager;
struct FTargetingCardData;
struct FTargetingResult;
struct FTargetingStepCardData;
struct FTargetingStepContext;

struct FTargetingPreviewContext
{
	ABattleCharacterBase* SourceCharacter = nullptr;
	ABattleGridManager* GridManager = nullptr;
	const FTargetingCardData* TargetingData = nullptr;
	const FTargetingStepCardData* StepData = nullptr;
	const FTargetingStepContext* StepContext = nullptr;
	const FTargetingResult* PreviewResult = nullptr;
	int32 StepIndex = INDEX_NONE;

	bool IsValid() const
	{
		return GridManager && TargetingData && StepData && StepContext;
	}

	void Reset()
	{
		SourceCharacter = nullptr;
		GridManager = nullptr;
		TargetingData = nullptr;
		StepData = nullptr;
		StepContext = nullptr;
		PreviewResult = nullptr;
		StepIndex = INDEX_NONE;
	}
};