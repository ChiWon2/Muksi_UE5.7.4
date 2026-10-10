#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "EnemyBattleAITypes.generated.h"

class UMuksiBattleCardDataAsset;

UENUM()
enum class EEnemySkillSelectState : uint8
{
	Failed,
	Selected,
	NoUsableSkill
};


USTRUCT(BlueprintType)
struct FEnemySkillSelectResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EEnemySkillSelectState State = EEnemySkillSelectState::Failed;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SelectedSkill = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FHexOffsetCoord> TargetingStepCoords;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> TargetingStepDirections;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid SelectedSkillInstanceId;
};


