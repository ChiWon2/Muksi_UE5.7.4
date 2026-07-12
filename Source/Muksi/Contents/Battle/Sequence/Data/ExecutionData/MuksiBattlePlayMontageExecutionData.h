#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"
#include "MuksiBattlePlayMontageExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiBattlePlayMontageExecutionData : public FMuksiBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName AnimKey = NAME_None;
};