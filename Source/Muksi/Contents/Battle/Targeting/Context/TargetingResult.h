#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"
#include "TargetingResult.generated.h"

USTRUCT(BlueprintType)
struct FTargetingResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepResult> Steps;

	bool HasResult() const { return !Steps.IsEmpty(); }

	const FTargetingStepResult* GetStep(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr;
	}

	FTargetingStepResult* GetMutableStep(int32 StepIndex)
	{
		return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr;
	}

	const FTargetingStepResult* GetLastStep() const
	{
		return Steps.IsEmpty() ? nullptr : &Steps.Last();
	}

	void Reset()
	{
		Steps.Empty();
	}
};
