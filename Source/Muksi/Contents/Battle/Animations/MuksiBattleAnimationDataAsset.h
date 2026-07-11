#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiTypes/MuksiWeaponTypes.h"
#include "MuksiBattleAnimationTypes.h"
#include "MuksiBattleAnimationDataAsset.generated.h"

UCLASS(BlueprintType)
class MUKSI_API UMuksiBattleAnimationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Animation")
	TArray<FMuksiWeaponBattleAnimationSet> WeaponAnimationSets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Animation|Common")
	TMap<FName, TObjectPtr<UAnimMontage>> CommonMontageMap;


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Battle Animation")
	UAnimMontage* FindMontage(FName AnimKey, EMuksiWeaponTypes WeaponType = EMuksiWeaponTypes::None) const;
};