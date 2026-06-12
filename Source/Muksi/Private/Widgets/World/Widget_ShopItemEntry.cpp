#include "Widgets/World/Widget_ShopItemEntry.h"

#include "Items/Data/MuksiItemDataAsset.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Widget.h"

void UWidget_ShopItemEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Root)
	{
		Button_Root->OnClicked.AddDynamic(this, &UWidget_ShopItemEntry::HandleClicked);
	}
}

void UWidget_ShopItemEntry::Setup(const FShopItemEntry& InEntry)
{
	Entry = InEntry;

	if (Entry.ItemData && Text_ItemName)
	{
		Text_ItemName->SetText(Entry.ItemData->DisplayName);
	}

	if (Text_Price)
	{
		Text_Price->SetText(FText::AsNumber(Entry.Price));
	}

	if (Text_Condition)
	{
		Text_Condition->SetText(Entry.RequiredConditionText);
	}

	if (ItemIconImage)
	{
		const bool bHasIcon = Entry.ItemData && Entry.ItemData->Icon;
		ItemIconImage->SetVisibility(bHasIcon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);

		if (bHasIcon)
		{
			ItemIconImage->SetBrushFromTexture(Entry.ItemData->Icon);
		}
	}

	SetSelected(false);
}

void UWidget_ShopItemEntry::SetSelected(bool bInSelected)
{
	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(bInSelected ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UWidget_ShopItemEntry::HandleClicked()
{
	OnClicked.ExecuteIfBound(Entry);
}