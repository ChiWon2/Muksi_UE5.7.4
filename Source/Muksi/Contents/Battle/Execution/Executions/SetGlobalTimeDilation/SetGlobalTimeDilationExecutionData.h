#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "SetGlobalTimeDilationExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FSetGlobalTimeDilationExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Time")
	float TimeDilation = 1.0f;
};
