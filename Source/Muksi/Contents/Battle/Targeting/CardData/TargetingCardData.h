#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"

#include "TargetingCardData.generated.h"

USTRUCT(BlueprintType)
struct FTargetingCardData
{
	GENERATED_BODY()

	/** Targeting은 위에서 아래 순서로 진행된다. 각 Step은 Origin / Selection / Intent / Resolve / Pattern / Presentation으로 구성된다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (DisplayName = "Targeting Steps"))
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