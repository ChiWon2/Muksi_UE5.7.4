#include "Muksi/Contents/Travel/Public/Widgets/Common/Widget_ItemTooltip.h"

#include "Components/TextBlock.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

void UWidget_ItemTooltip::InitializeTooltip(const FMuksiInventoryEntry& InEntry)
{
	const UMuksiItemDataAsset* ItemData = InEntry.ItemData;

	if (ItemNameText)
	{
		if (ItemData && ItemData->ItemType == EMuksiItemType::Equipment && InEntry.EnhanceLevel > 0)
		{
			ItemNameText->SetText(FText::FromString(
				FString::Printf(TEXT("%s +%d"), *ItemData->DisplayName.ToString(), InEntry.EnhanceLevel)
			));
		}
		else
		{
			ItemNameText->SetText(ItemData ? ItemData->DisplayName : FText::GetEmpty());
		}
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(ItemData ? ItemData->Description : FText::GetEmpty());
	}

	if (EquipmentStatsText)
	{
		const bool bIsEquipment = ItemData && ItemData->ItemType == EMuksiItemType::Equipment;
		EquipmentStatsText->SetVisibility(bIsEquipment ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		EquipmentStatsText->SetText(bIsEquipment
			? FText::FromString(BuildEquipmentStatsText(ItemData->EquipmentStatModifier))
			: FText::GetEmpty());
	}
}

FString UWidget_ItemTooltip::BuildEquipmentStatsText(const FMuksiItemStatModifier& Modifier) const
{
	TArray<FString> Lines;

	if (!FMath::IsNearlyZero(Modifier.AttackPower)) Lines.Add(FString::Printf(TEXT("Attack +%.0f"), Modifier.AttackPower));
	if (!FMath::IsNearlyZero(Modifier.DefensePower)) Lines.Add(FString::Printf(TEXT("Defense +%.0f"), Modifier.DefensePower));
	if (!FMath::IsNearlyZero(Modifier.CriticalRate)) Lines.Add(FString::Printf(TEXT("Critical +%.0f"), Modifier.CriticalRate));
	if (!FMath::IsNearlyZero(Modifier.HitRate)) Lines.Add(FString::Printf(TEXT("Hit Rate +%.0f"), Modifier.HitRate));
	if (!FMath::IsNearlyZero(Modifier.MoveSpeed)) Lines.Add(FString::Printf(TEXT("Move Speed +%.0f"), Modifier.MoveSpeed));
	if (!FMath::IsNearlyZero(Modifier.MaxHP)) Lines.Add(FString::Printf(TEXT("Max HP +%.0f"), Modifier.MaxHP));

	return FString::Join(Lines, TEXT("\n"));
}