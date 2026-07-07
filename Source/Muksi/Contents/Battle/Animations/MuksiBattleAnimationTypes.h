#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "MuksiTypes/MuksiWeaponTypes.h"
#include "MuksiBattleAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FMuksiWeaponBattleAnimationSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Animation")
	EMuksiWeaponTypes WeaponType = EMuksiWeaponTypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Animation")
	TMap<FName, TObjectPtr<UAnimMontage>> MontageMap;
};