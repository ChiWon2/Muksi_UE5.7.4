#include "MuksiBattleFXDataAsset.h"

const FMuksiBattleFXData* UMuksiBattleFXDataAsset::FindFXData(FName FXKey) const
{
	if (FXKey.IsNone())
		return nullptr;

	return FXMap.Find(FXKey);
}

void UMuksiBattleFXDataAsset::CollectFXAssetPaths(const TSet<FName>& FXKeys, TArray<FSoftObjectPath>& OutAssetPaths) const
{
	for (const TPair<FName, FMuksiBattleFXData>& FXPair : FXMap)
	{
		if (FXKeys.Contains(FXPair.Key) && !FXPair.Value.NiagaraSystem.IsNull())
			OutAssetPaths.AddUnique(FXPair.Value.NiagaraSystem.ToSoftObjectPath());
	}
}
