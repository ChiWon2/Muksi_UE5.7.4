#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MuksiTypes/MuksiWeaponTypes.h"
#include "MuksiBattleAnimInstance.generated.h"

UCLASS()
class MUKSI_API UMuksiBattleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Animation")
	EMuksiWeaponTypes CurrentWeaponType = EMuksiWeaponTypes::None;
};