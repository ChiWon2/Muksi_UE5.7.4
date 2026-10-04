#include "Muksi/Contents/Battle/Camera/MuksiBattleCameraDataAsset.h"

const TSoftObjectPtr<ULevelSequence>* UMuksiBattleCameraDataAsset::FindCameraSequence(FName CameraKey) const
{
	if (CameraKey.IsNone())
		return nullptr;

	return CameraMap.Find(CameraKey);
}
