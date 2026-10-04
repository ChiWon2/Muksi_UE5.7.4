#include "Muksi/Contents/Battle/Camera/MuksiBattleCameraDataAsset.h"

const FMuksiBattleCameraData* UMuksiBattleCameraDataAsset::FindCameraData(FName CameraKey) const
{
	if (CameraKey.IsNone())
		return nullptr;

	return CameraMap.Find(CameraKey);
}
