#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "RotateExecutionData.generated.h"

UENUM(BlueprintType)
enum class ERotateExecutionTargetMode : uint8
{
	TargetingResult,
	Opponent
};

USTRUCT(BlueprintType)
struct FRotateExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation")
	ERotateExecutionTargetMode TargetMode = ERotateExecutionTargetMode::TargetingResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation", meta = (ClampMin = "0.0"))
	float RotationSpeed = 720.0f;
};
