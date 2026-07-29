#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"

#include "TargetingCardData.generated.h"

USTRUCT(BlueprintType)
struct FTargetingCardData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepCardData> Steps;

	bool HasSteps() const
	{
		return !Steps.IsEmpty();
	}

	bool IsValidStepIndex(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex);
	}

	const FTargetingStepCardData* GetStep(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr;
	}

#if WITH_EDITOR
	void SyncDataTypes();
	void SyncStepDataTypes();
#endif
};