#include "MuksiBattleFXDataAsset.h"

const FMuksiBattleFXData* UMuksiBattleFXDataAsset::FindFXData(FName FXKey) const
{
	if (FXKey.IsNone())
		return nullptr;

	return FXMap.Find(FXKey);
}
