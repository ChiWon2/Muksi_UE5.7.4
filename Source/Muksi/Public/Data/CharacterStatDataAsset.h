#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiTypes/CharacterStatTypes.h"
#include "CharacterStatDataAsset.generated.h"

UCLASS(BlueprintType)
class MUKSI_API UCharacterStatDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FBaseStat BaseStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FWeaponMasteryStat WeaponMasteryStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FSpecialStat SpecialStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FTravelStat BaseTravelStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FResourceStat InitialResourceStat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	FCombatStat BaseCombatStat;
};
