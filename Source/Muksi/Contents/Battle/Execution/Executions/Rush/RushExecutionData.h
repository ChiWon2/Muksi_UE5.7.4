#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "RushExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FRushExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rush", meta = (ClampMin = "0.0"))
	float MoveSpeed = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rush")
	bool bRotateTowardPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rush|Animation")
	FName EndSection = NAME_None;
};
