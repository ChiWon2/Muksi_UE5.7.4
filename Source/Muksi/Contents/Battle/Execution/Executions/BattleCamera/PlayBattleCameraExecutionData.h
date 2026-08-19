#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "PlayBattleCameraExecutionData.generated.h"


USTRUCT(BlueprintType)
struct FPlayBattleCameraExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<ULevelSequence> LevelSequence = nullptr;
};