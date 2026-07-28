#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopInventoryPanel.h"

#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"
#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"
#include "Muksi/Contents/Travel/Public/Widgets/Common/Widget_ItemSlot.h"

void UWidget_ShopInventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();

	CachedCurrencyComponent = GetCurrencyComponent();

	if (CachedCurrencyComponent)
	{
		CachedCurrencyComponent->OnGoldChanged.AddUniqueDynamic(
			this,
			&ThisClass::HandleGoldChanged);
	}

	Refresh();
}

void UWidget_ShopInventoryPanel::NativeDestruct()
{
	if (CachedCurrencyComponent)
	{
		CachedCurrencyComponent->OnGoldChanged.RemoveDynamic(
			this,
			&ThisClass::HandleGoldChanged);

		CachedCurrencyComponent = nullptr;
	}

	Super::NativeDestruct();
}

void UWidget_ShopInventoryPanel::Refresh()
{
	RefreshGold();
	RefreshInventory();
}

void UWidget_ShopInventoryPanel::SetPanelMode(EShopInventoryPanelMode InMode)
{
	PanelMode = InMode;
	RefreshInventory();
}

void UWidget_ShopInventoryPanel::SetItemTypeFilter(EMuksiItemType InItemTypeFilter)
{
	ItemTypeFilter = InItemTypeFilter;
	RefreshInventory();
}

void UWidget_ShopInventoryPanel::SetSelectedInstanceId(FGuid InSelectedInstanceId)
{
	SelectedInstanceId = InSelectedInstanceId;
	RefreshInventory();
}

void UWidget_ShopInventoryPanel::RefreshInventory()
{
	if (!InventoryGridPanel)
	{
		return;
	}

	InventoryGridPanel->ClearChildren();

	const UInventoryComponent* InventoryComponent = GetInventoryComponent();

	if (!InventoryComponent || !ItemSlotClass)
	{
		return;
	}

	const int32 SafeColumnCount = FMath::Max(1, InventoryColumnCount);
	int32 VisibleIndex = 0;

	for (const FMuksiInventoryEntry& Entry : InventoryComponent->GetItems())
	{
		if (!ShouldShowEntry(Entry))
		{
			continue;
		}

		UWidget_ItemSlot* SlotWidget =
			CreateWidget<UWidget_ItemSlot>(GetOwningPlayer(), ItemSlotClass);

		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->InitializeSlot(Entry, false);
		SlotWidget->SetSelected(Entry.InstanceId == SelectedInstanceId);

		if (PanelMode == EShopInventoryPanelMode::Sell)
		{
			SlotWidget->OnClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
		}

		const int32 Row = VisibleIndex / SafeColumnCount;
		const int32 Column = VisibleIndex % SafeColumnCount;

		InventoryGridPanel->AddChildToUniformGrid(SlotWidget, Row, Column);
		++VisibleIndex;
	}
}

void UWidget_ShopInventoryPanel::HandleItemSlotClicked(FGuid InstanceId)
{
	if (!InstanceId.IsValid())
	{
		return;
	}

	SelectedInstanceId = InstanceId;
	OnItemSelected.Broadcast(InstanceId);

	RefreshInventory();
}

void UWidget_ShopInventoryPanel::RefreshGold()
{
	const UPlayerCurrencyComponent* CurrencyComponent =
		CachedCurrencyComponent
		? CachedCurrencyComponent.Get()
		: GetCurrencyComponent();

	if (Text_Gold)
	{
		Text_Gold->SetText(FText::AsNumber(
			CurrencyComponent ? CurrencyComponent->GetGold() : 0));
	}
}

bool UWidget_ShopInventoryPanel::ShouldShowEntry(const FMuksiInventoryEntry& Entry) const
{
	if (!Entry.ItemData || !Entry.InstanceId.IsValid())
	{
		return false;
	}

	if (ItemTypeFilter != EMuksiItemType::None &&
		Entry.ItemData->ItemType != ItemTypeFilter)
	{
		return false;
	}

	if (PanelMode == EShopInventoryPanelMode::Sell)
	{
		if (!Entry.ItemData->bCanSell)
		{
			return false;
		}

		if (Entry.ItemData->ItemType == EMuksiItemType::Quest)
		{
			return false;
		}

		switch (Entry.ItemData->ItemType)
		{
		case EMuksiItemType::Equipment:
		case EMuksiItemType::Consumable:
		case EMuksiItemType::Material:
		case EMuksiItemType::Misc:
			return true;

		default:
			return false;
		}
	}

	return true;
}

void UWidget_ShopInventoryPanel::HandleGoldChanged(int32 NewGold)
{
	if (Text_Gold)
	{
		Text_Gold->SetText(FText::AsNumber(NewGold));
	}
}

UInventoryComponent* UWidget_ShopInventoryPanel::GetInventoryComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this);

	return PlayerData
		? PlayerData->GetPlayerInventoryComponent()
		: nullptr;
}

UPlayerCurrencyComponent* UWidget_ShopInventoryPanel::GetCurrencyComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this);

	return PlayerData
		? PlayerData->GetPlayerCurrencyComponent()
		: nullptr;
}