#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

UMuksiItemDataAsset::UMuksiItemDataAsset()
{
	ItemType = EMuksiItemType::None;
	EquipmentSlot = EMuksiEquipmentSlot::None;
	MaxStackCount = 1;
	bStackable = false;
	Price = 0;
}
