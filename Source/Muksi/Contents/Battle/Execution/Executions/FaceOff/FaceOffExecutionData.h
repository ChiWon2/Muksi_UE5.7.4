#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "FaceOffExecutionData.generated.h"

UENUM(BlueprintType)
enum class EFaceOffMoveMode : uint8
{
	AttackerToTarget,
	TargetToAttacker,
	Both
};

USTRUCT(BlueprintType)
struct FFaceOffExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff")
	EBattleExecutionTargetPolicy TargetPolicy = EBattleExecutionTargetPolicy::TargetingResult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff")
	EFaceOffMoveMode MoveMode = EFaceOffMoveMode::Both;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff", meta = (ClampMin = "0.0"))
	float CharacterDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff", meta = (ClampMin = "0.0"))
	float MoveDuration = 0.2f;
};
