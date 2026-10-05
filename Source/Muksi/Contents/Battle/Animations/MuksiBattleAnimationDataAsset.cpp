#include "MuksiBattleAnimationDataAsset.h"

UAnimMontage* UMuksiBattleAnimationDataAsset::FindMontage(FName AnimKey, EMuksiWeaponTypes WeaponType) const
{
	if (AnimKey.IsNone())
		return nullptr;

	for (const FMuksiWeaponBattleAnimationSet& WeaponSet : WeaponAnimationSets)
	{
		if (WeaponSet.WeaponType != WeaponType)
			continue;

		if (const TSoftObjectPtr<UAnimMontage>* FoundMontage = WeaponSet.MontageMap.Find(AnimKey))
			return FoundMontage->LoadSynchronous();

		break;
	}

	if (const TSoftObjectPtr<UAnimMontage>* FoundCommonMontage = CommonMontageMap.Find(AnimKey))
		return FoundCommonMontage->LoadSynchronous();

	return nullptr;
}

void UMuksiBattleAnimationDataAsset::CollectMontageAssetPaths(const TSet<FName>& AnimKeys, TArray<FSoftObjectPath>& OutAssetPaths) const
{
	for (const FMuksiWeaponBattleAnimationSet& WeaponSet : WeaponAnimationSets)
	{
		for (const TPair<FName, TSoftObjectPtr<UAnimMontage>>& MontagePair : WeaponSet.MontageMap)
		{
			if (AnimKeys.Contains(MontagePair.Key) && !MontagePair.Value.IsNull())
				OutAssetPaths.AddUnique(MontagePair.Value.ToSoftObjectPath());
		}
	}

	for (const TPair<FName, TSoftObjectPtr<UAnimMontage>>& MontagePair : CommonMontageMap)
	{
		if (AnimKeys.Contains(MontagePair.Key) && !MontagePair.Value.IsNull())
			OutAssetPaths.AddUnique(MontagePair.Value.ToSoftObjectPath());
	}
}
