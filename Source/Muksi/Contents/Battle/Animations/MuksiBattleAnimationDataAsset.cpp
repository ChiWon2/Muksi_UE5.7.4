#include "MuksiBattleAnimationDataAsset.h"

UAnimMontage* UMuksiBattleAnimationDataAsset::FindMontage(FName AnimKey, EMuksiWeaponTypes WeaponType) const
{
	if (AnimKey.IsNone())
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

		break;
	}

	if (const TObjectPtr<UAnimMontage>* FoundCommonMontage = CommonMontageMap.Find(AnimKey))
	{
		return FoundCommonMontage->Get();
	}

	return nullptr;
}