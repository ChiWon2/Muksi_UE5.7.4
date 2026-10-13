#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleEncounterData.generated.h"

class UMuksiCharacterDataAsset;

// 전투 진입 시 필요한 Encounter 정보.
USTRUCT(BlueprintType)
struct FMuksiBattleEncounterData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
    TSoftObjectPtr<UMuksiCharacterDataAsset> EnemyCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
    TSoftObjectPtr<UWorld> BattleLevel;
};
