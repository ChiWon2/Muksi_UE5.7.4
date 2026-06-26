#include "Muksi/Contents/Travel/Public/Widgets/Common/Widget_ItemSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Input/Reply.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"
#include "Muksi/Contents/Travel/Public/Widgets/Common/Widget_ItemTooltip.h"

void UWidget_ItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.RemoveAll(this);
		SlotButton->OnClicked.AddDynamic(this, &ThisClass::HandleSlotClicked);
	}

	Refresh();
	RefreshTooltip();
}

void UWidget_ItemSlot::InitializeSlot(const FMuksiInventoryEntry& InEntry, bool bInEquipped)
{
	Entry = InEntry;
	bEquipped = bInEquipped;

	Refresh();
	RefreshTooltip();
}

void UWidget_ItemSlot::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	Refresh();
}

FReply UWidget_ItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!Entry.InstanceId.IsValid())
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnClicked.Broadcast(Entry.InstanceId);
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClicked.Broadcast(Entry.InstanceId);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UWidget_ItemSlot::HandleSlotClicked()
{
	if (Entry.InstanceId.IsValid())
	{
		OnClicked.Broadcast(Entry.InstanceId);
	}
}

void UWidget_ItemSlot::Refresh()
{
	const UMuksiItemDataAsset* ItemData = Entry.ItemData;

	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		SelectedBorder->SetBrushColor(bSelected ? FLinearColor(1.f, 0.85f, 0.05f, 1.f) : FLinearColor::Transparent);
	}

	if (ItemIconImage)
	{
		const bool bHasIcon = ItemData && ItemData->Icon;
		ItemIconImage->SetVisibility(bHasIcon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		if (bHasIcon)
		{
			ItemIconImage->SetBrushFromTexture(ItemData->Icon);
		}
	}

	if (QuantityText)
	{
		const bool bShowQuantity = ItemData && Entry.Quantity > 1;
		QuantityText->SetVisibility(bShowQuantity ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		QuantityText->SetText(FText::AsNumber(Entry.Quantity));
	}

	if (EquippedText)
	{
		EquippedText->SetVisibility(bEquipped ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		EquippedText->SetText(FText::FromString(TEXT("E")));
	}

	if (EnhanceLevelText)
	{
		const bool bShowEnhanceLevel =
			ItemData &&
			ItemData->ItemType == EMuksiItemType::Equipment &&
			Entry.EnhanceLevel > 0;

		EnhanceLevelText->SetVisibility(bShowEnhanceLevel ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (bShowEnhanceLevel)
		{
			EnhanceLevelText->SetText(FText::FromString(
				FString::Printf(TEXT("+%d"), Entry.EnhanceLevel)
			));
		}
	}

	if (SlotButton)
	{
		SlotButton->SetIsEnabled(ItemData != nullptr);
	}
}

void UWidget_ItemSlot::RefreshTooltip()
{
	if (!TooltipClass || !Entry.ItemData)
	{
		SetToolTip(nullptr);
		return;
	}

	UWidget_ItemTooltip* TooltipWidget = CreateWidget<UWidget_ItemTooltip>(GetOwningPlayer(), TooltipClass);
	if (!TooltipWidget)
	{
		SetToolTip(nullptr);
		return;
	}

	TooltipWidget->InitializeTooltip(Entry);
	SetToolTip(TooltipWidget);
}
