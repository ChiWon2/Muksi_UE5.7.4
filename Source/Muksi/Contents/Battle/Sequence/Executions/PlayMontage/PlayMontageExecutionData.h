#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "PlayMontageExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FPlayMontageExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName AnimKey = NAME_None;
};