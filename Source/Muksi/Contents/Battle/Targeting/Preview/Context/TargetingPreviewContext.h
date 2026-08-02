#pragma once

#include "CoreMinimal.h"

class ABattleGridManager;
struct FResolvedTargeting;
struct FTargetingStepCardData;
struct FTargetingStepResult;

struct FTargetingPreviewContext
{
	ABattleGridManager* GridManager = nullptr;
	const FTargetingStepCardData* StepData = nullptr;
	const FTargetingStepResult* StepResult = nullptr;
	const FResolvedTargeting* ResolvedTargeting = nullptr;
	FVector AimWorldLocation = FVector::ZeroVector;
	bool bHasAimWorldLocation = false;


	bool IsValid() const
	{
		return GridManager && StepData && StepResult;
	}
};
