#include "MuksiBattleAnimationDataAsset.h"

UAnimMontage* UMuksiBattleAnimationDataAsset::FindMontage(EMuksiWeaponTypes WeaponType,FName AnimKey) const
{
	if (WeaponType == EMuksiWeaponTypes::None || AnimKey.IsNone())
	{
		return nullptr;
	}

	for (const FMuksiWeaponBattleAnimationSet& WeaponSet : WeaponAnimationSets)
	{
		if (WeaponSet.WeaponType != WeaponType)
		{
			continue;
		}

		if (const TObjectPtr<UAnimMontage>* FoundMontage = WeaponSet.MontageMap.Find(AnimKey))
		{
			return FoundMontage->Get();
		}

		return nullptr;
	}

	return nullptr;
}