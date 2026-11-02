#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/CharacterStatTypes.h"
#include "CharacterStatDataAsset.generated.h"

class UCharacterPassive;
class UMuksiBattleCardDataAsset;

UCLASS(BlueprintType)
class MUKSI_API UCharacterStatDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FBaseStat BaseStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<UMuksiBattleCardDataAsset*> BaseSkillDataAssets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passive")
	TArray<TSubclassOf<UCharacterPassive>> BasePassives;
};
