#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "MoveExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FMoveExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	EMuksiBattleMoveType MoveType = EMuksiBattleMoveType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ground", meta = (EditCondition = "MoveType == EMuksiBattleMoveType::GroundPath", EditConditionHides, ClampMin = "0.0"))
	float GroundMoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ground", meta = (EditCondition = "MoveType == EMuksiBattleMoveType::GroundPath", EditConditionHides))
	FName GroundPathEndSection = TEXT("Run_End");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (EditCondition = "MoveType == EMuksiBattleMoveType::Jump", EditConditionHides, ClampMin = "0.0"))
	float JumpDuration = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (EditCondition = "MoveType == EMuksiBattleMoveType::Jump", EditConditionHides, ClampMin = "0.0"))
	float JumpArcHeight = 200.0f;
};