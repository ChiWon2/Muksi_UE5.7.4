#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_Shop.h"

#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopItemEntry.h"
#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopInventoryPanel.h"

#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"
#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/EquipmentComponent.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"
#include "Subsystems/MuksiShopSubsystem.h"
#include "Subsystems/MuksiItemDataSubsystem.h"
#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"


#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UWidget_Shop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_BuyOrSell)
	{
		Button_BuyOrSell->OnClicked.RemoveAll(this);
		Button_BuyOrSell->OnClicked.AddDynamic(this, &ThisClass::HandleActionClicked);
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.RemoveAll(this);
		Button_Close->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
	}

	if (Button_TabBuy)
	{
		Button_TabBuy->OnClicked.RemoveAll(this);
		Button_TabBuy->OnClicked.AddDynamic(this, &ThisClass::HandleBuyTabClicked);
	}

	if (Button_TabSell)
	{
		Button_TabSell->OnClicked.RemoveAll(this);
		Button_TabSell->OnClicked.AddDynamic(this, &ThisClass::HandleSellTabClicked);
	}

	if (Button_TabRebuy)
	{
		Button_TabRebuy->OnClicked.RemoveAll(this);
		Button_TabRebuy->OnClicked.AddDynamic(this, &ThisClass::HandleRebuyTabClicked);
	}

	RefreshTabContent();
	RefreshSelection();

	if (ShopInventoryPanel)
	{
		ShopInventoryPanel->Refresh();
	}
}

void UWidget_Shop::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent())
	{
		CurrencyComponent->OnGoldChanged.RemoveAll(this);
		CurrencyComponent->OnGoldChanged.AddDynamic(this, &ThisClass::HandleGoldChanged);
	}

	if (ShopInventoryPanel)
	{
		ShopInventoryPanel->OnItemSelected.RemoveAll(this);
		ShopInventoryPanel->OnItemSelected.AddDynamic(
			this,
			&ThisClass::HandleSellInventoryItemSelected);
	}

	bHasSelectedSellItem = false;
	SelectedSellItem = FMuksiInventoryEntry();
	SelectedSellInstanceId.Invalidate();

	RefreshTabContent();
	RefreshSelection();
	RefreshActionButtonText();
}

void UWidget_Shop::NativeOnDeactivated()
{
	if (UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent())
	{
		CurrencyComponent->OnGoldChanged.RemoveAll(this);
	}

	bHasSelectedItem = false;
	SelectedShopItem = FShopItemEntry();

	Super::NativeOnDeactivated();
}

void UWidget_Shop::SetShopData(UShopDataAsset* InShopData)
{
	ShopData = InShopData;

	if (UMuksiShopSubsystem* ShopSubsystem =
		GetShopSubsystem())
	{
		ShopSubsystem->InitializeShopIfNeeded(ShopData);
	}

	if (IsActivated())
	{
		RefreshBuyItems();
		RefreshSelection();
	}
}

int32 UWidget_Shop::GetRemainingStock(const FShopItemEntry& Entry) const
{
	if (Entry.bUnlimitedStock)
	{
		return MAX_int32;
	}

	const UMuksiShopSubsystem* ShopSubsystem =
		GetShopSubsystem();

	return ShopSubsystem
		? ShopSubsystem->GetRemainingStock(
			ShopData,
			Entry.ShopEntryId)
		: 0;
}

UMuksiShopSubsystem* UWidget_Shop::GetShopSubsystem() const
{
	return UMuksiShopSubsystem::Get(this);
}

void UWidget_Shop::RefreshBuyItems()
{
	if (!ItemGridPanel)
	{
		return;
	}

	ItemGridPanel->ClearChildren();

	if (Text_ShopTitle)
	{
		Text_ShopTitle->SetText(ShopData ? ShopData->ShopName : FText::FromString(TEXT("Shop")));
	}

	if (!ShopData || !ShopItemEntryClass)
	{
		return;
	}

	const int32 SafeColumnCount = FMath::Max(1, ShopItemColumnCount);
	int32 VisibleIndex = 0;

	for (const FShopItemEntry& Entry : ShopData->SellItems)
	{
		UMuksiItemDataAsset* ItemData = ResolveShopItemData(Entry);
		if (!ItemData)
		{
			continue;
		}

		UWidget_ShopItemEntry* EntryWidget =
			CreateWidget<UWidget_ShopItemEntry>(GetOwningPlayer(), ShopItemEntryClass);

		if (!EntryWidget)
		{
			continue;
		}

		EntryWidget->Setup(Entry, ItemData);
		EntryWidget->SetSelected(
			bHasSelectedItem &&
			SelectedShopItem.ShopEntryId == Entry.ShopEntryId);

		EntryWidget->OnClicked.BindUObject(this, &ThisClass::SelectShopItem);

		const int32 Row = VisibleIndex / SafeColumnCount;
		const int32 Column = VisibleIndex % SafeColumnCount;

		ItemGridPanel->AddChildToUniformGrid(EntryWidget, Row, Column);
		++VisibleIndex;
	}
}	

void UWidget_Shop::SelectShopItem(const FShopItemEntry& Entry)
{
	UMuksiItemDataAsset* ItemData = ResolveShopItemData(Entry);

	UE_LOG(LogTemp, Log, TEXT("[Shop] SelectShopItem ItemID=%s Item=%s Price=%d"),
		*Entry.ItemID.ToString(),
		*GetNameSafe(ItemData),
		Entry.Price);

	SelectedShopItem = Entry;
	bHasSelectedItem = true;

	RefreshBuyItems();
	RefreshSelection();
}

void UWidget_Shop::RefreshSelection()
{
	const UPlayerCurrencyComponent* CurrencyComponent =
		GetCurrencyComponent();

	const int32 CurrentGold =
		CurrencyComponent ? CurrencyComponent->GetGold() : 0;

	const bool bCanBuy = CanBuySelectedItem();

	UMuksiItemDataAsset* SelectedItemData =
		bHasSelectedItem ? ResolveShopItemData(SelectedShopItem) : nullptr;

	if (Text_CurrentGold)
	{
		Text_CurrentGold->SetText(FText::AsNumber(CurrentGold));
	}

	if (CurrentTab == EShopTab::Sell)
	{
		const bool bCanSell = CanSellSelectedItem();
		const int32 SellPrice = bHasSelectedSellItem ? GetSellPrice(SelectedSellItem) : 0;

		if (Text_SelectedItemName)
		{
			Text_SelectedItemName->SetText(
				bHasSelectedSellItem && SelectedSellItem.ItemData
				? SelectedSellItem.ItemData->DisplayName
				: FText::FromString(TEXT("-")));
		}

		if (Text_SelectedItemDescription)
		{
			Text_SelectedItemDescription->SetText(
				bHasSelectedSellItem && SelectedSellItem.ItemData
				? SelectedSellItem.ItemData->Description
				: FText::GetEmpty());
		}

		if (Text_Price)
		{
			Text_Price->SetText(FText::AsNumber(SellPrice));
		}

		if (Text_AfterGold)
		{
			Text_AfterGold->SetText(FText::AsNumber(CurrentGold + SellPrice));
		}

		if (Button_BuyOrSell)
		{
			Button_BuyOrSell->SetIsEnabled(bCanSell);
		}

		return;
	}

	if (!bHasSelectedItem || !SelectedItemData)
	{
		if (Text_SelectedItemName)
		{
			Text_SelectedItemName->SetText(FText::FromString(TEXT("-")));
		}

		if (Text_SelectedItemDescription)
		{
			Text_SelectedItemDescription->SetText(FText::GetEmpty());
		}

		if (Text_Price)
		{
			Text_Price->SetText(FText::AsNumber(0));
		}

		if (Text_AfterGold)
		{
			Text_AfterGold->SetText(FText::AsNumber(CurrentGold));
		}

		if (Button_BuyOrSell)
		{
			Button_BuyOrSell->SetIsEnabled(false);
		}

		return;
	}

	if (Text_SelectedItemName)
	{
		Text_SelectedItemName->SetText(SelectedItemData->DisplayName);
	}

	if (Text_SelectedItemDescription)
	{
		Text_SelectedItemDescription->SetText(SelectedItemData->Description);
	}

	if (Text_Price)
	{
		Text_Price->SetText(FText::AsNumber(SelectedShopItem.Price));
	}

	if (Text_AfterGold)
	{
		const int32 AfterGold =
			bHasSelectedItem && SelectedItemData
			? FMath::Max(0, CurrentGold - SelectedShopItem.Price)
			: CurrentGold;

		Text_AfterGold->SetText(FText::AsNumber(AfterGold));
	}

	if (Button_BuyOrSell)
	{
		Button_BuyOrSell->SetIsEnabled(bCanBuy);
	}

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("[Widget_Shop] RefreshSelection Item=%s Price=%d Gold=%d CanBuy=%d"),
		*GetNameSafe(SelectedItemData),
		SelectedShopItem.Price,
		CurrentGold,
		bCanBuy
	);
}

bool UWidget_Shop::CanBuySelectedItem() const
{
	if (CurrentTab != EShopTab::Buy ||
		!bHasSelectedItem ||
		SelectedShopItem.ItemID.IsNone() ||
		SelectedShopItem.Price <= 0)
	{
		return false;
	}

	if (!ResolveShopItemData(SelectedShopItem))
	{
		return false;
	}

	const UInventoryComponent* InventoryComponent =
		GetInventoryComponent();

	const UPlayerCurrencyComponent* CurrencyComponent =
		GetCurrencyComponent();

	if (!InventoryComponent || !CurrencyComponent)
	{
		return false;
	}

	if (!CurrencyComponent->HasEnoughGold(SelectedShopItem.Price))
	{
		return false;
	}

	if (!SelectedShopItem.bUnlimitedStock &&
		GetRemainingStock(SelectedShopItem) <= 0)
	{
		return false;
	}

	return true;
}

bool UWidget_Shop::TryBuySelectedItem()
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();

	UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();

	UMuksiShopSubsystem* ShopSubsystem = GetShopSubsystem();

	UMuksiItemDataAsset* ItemData = ResolveShopItemData(SelectedShopItem);

	const int32 Price = SelectedShopItem.Price;
	const int32 GoldBefore = CurrencyComponent ? CurrencyComponent->GetGold() : -1;

	const bool bHasEnough = CurrencyComponent && Price > 0 && CurrencyComponent->HasEnoughGold(Price);

	bool bStockReserved = false;

	UE_LOG(LogTemp, Warning,
		TEXT("[Widget_Shop] Buy Try Selected=%d Item=%s Shop=%s Entry=%s Price=%d Gold=%d HasEnough=%d"),
		bHasSelectedItem,
		*GetNameSafe(ItemData),
		ShopData ? *ShopData->ShopId.ToString() : TEXT("None"),
		*SelectedShopItem.ShopEntryId.ToString(),
		Price,
		GoldBefore,
		bHasEnough);

	if (!bHasSelectedItem || !ItemData)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: no selected item")
		);
		return false;
	}

	if (Price <= 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: invalid price Price=%d"),
			Price
		);
		return false;
	}

	if (!InventoryComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: InventoryComponent is null")
		);
		return false;
	}

	if (!CurrencyComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: CurrencyComponent is null")
		);
		return false;
	}

	if (!CanBuySelectedItem())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: CanBuySelectedItem returned false")
		);
		return false;
	}

	if (!CurrencyComponent->HasEnoughGold(Price))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Widget_Shop] Buy Failed: not enough Gold Price=%d CurrentGold=%d"),
			Price,
			CurrencyComponent->GetGold()
		);
		return false;
	}

	if (!SelectedShopItem.bUnlimitedStock)
	{
		if (!ShopSubsystem)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[Widget_Shop] Buy Failed: ShopSubsystem is null"));
			return false;
		}

		bStockReserved = ShopSubsystem->TryReserveStock(
			ShopData,
			SelectedShopItem.ShopEntryId,
			1);

		if (!bStockReserved)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[Widget_Shop] Buy Failed: stock reservation failed Shop=%s Entry=%s"),
				ShopData ? *ShopData->ShopId.ToString() : TEXT("None"),
				*SelectedShopItem.ShopEntryId.ToString());

			return false;
		}
	}

	//Spend Gold
	const bool bSpendGoldSucceeded =
		CurrencyComponent->SpendGold(Price);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Widget_Shop] SpendGold Result=%d Price=%d GoldBefore=%d GoldAfter=%d"),
		bSpendGoldSucceeded,
		Price,
		GoldBefore,
		CurrencyComponent->GetGold()
	);

	if (!bSpendGoldSucceeded)
	{
		if (bStockReserved)
		{
			ShopSubsystem->ReleaseStock(
				ShopData,
				SelectedShopItem.ShopEntryId,
				1);
		}

		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Buy Failed: SpendGold failed. Stock released=%d"),
			bStockReserved);

		return false;
	}

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Buy Failed: ItemData not found. ItemID=%s"),
			*SelectedShopItem.ItemID.ToString());
		return false;
	}

	//Add Item
	const bool bAddItemSucceeded =
		InventoryComponent->AddItem(SelectedShopItem.ItemID, 1 ,ItemData);

	UE_LOG(LogTemp, Log,
		TEXT("[Widget_Shop] AddItem Result=%d ItemID=%s Item=%s"),
		bAddItemSucceeded,
		*SelectedShopItem.ItemID.ToString(),
		*GetNameSafe(ItemData));

	if (!bAddItemSucceeded)
	{
		CurrencyComponent->AddGold(Price);

		if (bStockReserved)
		{
			ShopSubsystem->ReleaseStock(
				ShopData,
				SelectedShopItem.ShopEntryId,
				1);
		}

		UE_LOG(LogTemp, Error,
			TEXT("[Widget_Shop] Buy rollback. AddItem failed GoldRefund=%d StockReleased=%d"),
			Price,
			bStockReserved);

		return false;
	}

	//RemainStock
	const int32 RemainingStock =
		GetRemainingStock(SelectedShopItem);

	UE_LOG(LogTemp, Log,
		TEXT("[Widget_Shop] Buy Success ItemID=%s Item=%s Shop=%s Entry=%s Price=%d GoldBefore=%d GoldAfter=%d Unlimited=%d RemainingStock=%d"),
		*SelectedShopItem.ItemID.ToString(),
		*GetNameSafe(ItemData),
		ShopData ? *ShopData->ShopId.ToString() : TEXT("None"),
		*SelectedShopItem.ShopEntryId.ToString(),
		Price,
		GoldBefore,
		CurrencyComponent->GetGold(),
		SelectedShopItem.bUnlimitedStock,
		RemainingStock);

	return true;
}

bool UWidget_Shop::CanSellSelectedItem() const
{
	if (CurrentTab != EShopTab::Sell ||
		!bHasSelectedSellItem ||
		!SelectedSellInstanceId.IsValid())
	{
		return false;
	}

	return IsSellableItem(SelectedSellItem) && GetSellPrice(SelectedSellItem) > 0;
}

bool UWidget_Shop::IsSellableItem(const FMuksiInventoryEntry& Entry) const
{
	if (!Entry.InstanceId.IsValid() || !Entry.ItemData)
	{
		return false;
	}

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
		break;

	default:
		return false;
	}

	if (Entry.ItemData->ItemType == EMuksiItemType::Equipment)
	{
		const UEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
		if (EquipmentComponent && EquipmentComponent->IsEquipped(Entry.InstanceId))
		{
			return false;
		}
	}

	return Entry.ItemData->Price > 0;
}

int32 UWidget_Shop::GetSellPrice(const FMuksiInventoryEntry& Entry) const
{
	if (!Entry.ItemData)
	{
		return 0;
	}

	if (Entry.ItemData->ItemType == EMuksiItemType::Quest)
	{
		return 0;
	}

	constexpr float SellPriceRate = 0.5f;
	return FMath::Max(1, FMath::FloorToInt(static_cast<float>(Entry.ItemData->Price) * SellPriceRate));
}

bool UWidget_Shop::TrySellSelectedItem()
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();

	if (!InventoryComponent || !CurrencyComponent)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Sell Failed: Inventory=%s Currency=%s"),
			*GetNameSafe(InventoryComponent),
			*GetNameSafe(CurrencyComponent));
		return false;
	}

	if (!bHasSelectedSellItem || !SelectedSellInstanceId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Widget_Shop] Sell Failed: no selected item"));
		return false;
	}

	FMuksiInventoryEntry CurrentEntry;
	if (!InventoryComponent->FindItemByInstanceId(SelectedSellInstanceId, CurrentEntry) || !CurrentEntry.ItemData)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Sell Failed: item not found InstanceId=%s"),
			*SelectedSellInstanceId.ToString());
		return false;
	}

	if (!IsSellableItem(CurrentEntry))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Sell Failed: not sellable ItemID=%s Type=%d"),
			*CurrentEntry.ItemID.ToString(),
			static_cast<int32>(CurrentEntry.ItemData->ItemType));
		return false;
	}

	const int32 SellQuantity = 1;
	const int32 SellPrice = GetSellPrice(CurrentEntry) * SellQuantity;
	const int32 GoldBefore = CurrencyComponent->GetGold();

	if (SellPrice <= 0)
	{
		return false;
	}

	if (!InventoryComponent->RemoveItemByInstanceId(SelectedSellInstanceId, SellQuantity))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_Shop] Sell Failed: RemoveItemByInstanceId failed InstanceId=%s"),
			*SelectedSellInstanceId.ToString());
		return false;
	}

	CurrencyComponent->AddGold(SellPrice);

	UE_LOG(LogTemp, Log,
		TEXT("[Widget_Shop] Sell Success InstanceId=%s ItemID=%s Quantity=%d SellPrice=%d GoldBefore=%d GoldAfter=%d"),
		*SelectedSellInstanceId.ToString(),
		*CurrentEntry.ItemID.ToString(),
		SellQuantity,
		SellPrice,
		GoldBefore,
		CurrencyComponent->GetGold());

	bHasSelectedSellItem = false;
	SelectedSellItem = FMuksiInventoryEntry();
	SelectedSellInstanceId.Invalidate();

	return true;
}

void UWidget_Shop::HandleActionClicked()
{
	bool bSucceeded = false;

	switch (CurrentTab)
	{
	case EShopTab::Buy:
		bSucceeded = TryBuySelectedItem();
		break;

	case EShopTab::Sell:
		bSucceeded = TrySellSelectedItem();
		break;

	default:
		break;
	}

	if (!bSucceeded)
	{
		RefreshSelection();
		return;
	}

	RefreshTabContent();
	RefreshSelection();

	if (ShopInventoryPanel)
	{
		ShopInventoryPanel->Refresh();
	}
}

void UWidget_Shop::HandleCloseClicked()
{
	DeactivateWidget();
}

void UWidget_Shop::HandleGoldChanged(int32 NewGold)
{
	UE_LOG(LogTemp, Log,
		TEXT("[Widget_Shop] Gold changed. NewGold=%d RefreshSelection=1"),
		NewGold);

	RefreshSelection();
}

void UWidget_Shop::HandleSellInventoryItemSelected(FGuid InstanceId)
{
	SelectSellItem(InstanceId);
}

void UWidget_Shop::SelectSellItem(FGuid InstanceId)
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent || !InstanceId.IsValid())
	{
		return;
	}

	FMuksiInventoryEntry FoundEntry;
	if (!InventoryComponent->FindItemByInstanceId(InstanceId, FoundEntry) || !FoundEntry.ItemData)
	{
		return;
	}

	SelectedSellInstanceId = InstanceId;
	SelectedSellItem = FoundEntry;
	bHasSelectedSellItem = true;

	if (ShopInventoryPanel)
	{
		ShopInventoryPanel->SetSelectedInstanceId(SelectedSellInstanceId);
		ShopInventoryPanel->Refresh();
	}

	RefreshSelection();

	UE_LOG(LogTemp, Log,
		TEXT("[Widget_Shop] SelectSellItem InstanceId=%s ItemID=%s Type=%d Quantity=%d SellPrice=%d"),
		*InstanceId.ToString(),
		*FoundEntry.ItemID.ToString(),
		static_cast<int32>(FoundEntry.ItemData->ItemType),
		FoundEntry.Quantity,
		GetSellPrice(FoundEntry));
}

UInventoryComponent* UWidget_Shop::GetInventoryComponent() const
{
	if (UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this))
	{
		return PlayerData->GetPlayerInventoryComponent();
	}

	return nullptr;
}

UEquipmentComponent* UWidget_Shop::GetEquipmentComponent() const
{
	if (UMuksiPlayerDataSubsystem* PlayerData = UMuksiPlayerDataSubsystem::Get(this))
	{
		return PlayerData->GetPlayerEquipmentComponent();
	}

	return nullptr;
}

UPlayerCurrencyComponent* UWidget_Shop::GetCurrencyComponent() const
{
	if (UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this))
	{
		return PlayerData->GetPlayerCurrencyComponent();
	}

	return nullptr;
}

void UWidget_Shop::HandleBuyTabClicked()
{
	SetCurrentTab(EShopTab::Buy);
}

void UWidget_Shop::HandleSellTabClicked()
{
	SetCurrentTab(EShopTab::Sell);
}

void UWidget_Shop::HandleRebuyTabClicked()
{
	SetCurrentTab(EShopTab::ReBuy);
}

void UWidget_Shop::SetCurrentTab(EShopTab NewTab)
{
	if (CurrentTab == NewTab)
	{
		RefreshTabContent();
		RefreshSelection();
		RefreshActionButtonText();
		return;
	}

	CurrentTab = NewTab;

	bHasSelectedItem = false;
	SelectedShopItem = FShopItemEntry();

	bHasSelectedSellItem = false;
	SelectedSellItem = FMuksiInventoryEntry();
	SelectedSellInstanceId.Invalidate();


	RefreshTabContent();
	RefreshSelection();
	RefreshActionButtonText();
}

void UWidget_Shop::RefreshTabContent()
{
	switch (CurrentTab)
	{
	case EShopTab::Buy:
		if (Text_EmptyMessage)
		{
			Text_EmptyMessage->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (ShopInventoryPanel)
		{
			ShopInventoryPanel->SetPanelMode(EShopInventoryPanelMode::ViewOnly);
			ShopInventoryPanel->SetItemTypeFilter(EMuksiItemType::None);
			ShopInventoryPanel->SetSelectedInstanceId(FGuid());
			ShopInventoryPanel->Refresh();
		}

		RefreshBuyItems();
		break;

	case EShopTab::Sell:
		ClearItemGrid();

		if (ShopInventoryPanel)
		{
			ShopInventoryPanel->SetPanelMode(EShopInventoryPanelMode::Sell);
			ShopInventoryPanel->SetItemTypeFilter(CurrentSellItemTypeFilter);
			ShopInventoryPanel->SetSelectedInstanceId(SelectedSellInstanceId);
			ShopInventoryPanel->Refresh();
		}

		ShowEmptyMessage(FText::FromString(TEXT("판매할 아이템을 선택하세요.")));
		break;

	case EShopTab::ReBuy:
		ClearItemGrid();
		ShowEmptyMessage(FText::FromString(TEXT("재구매 기능 준비중")));
		break;

	default:
		break;
	}
}

void UWidget_Shop::RefreshActionButtonText()
{
	if (!Text_ActionButton)
	{
		return;
	}

	switch (CurrentTab)
	{
	case EShopTab::Buy:
		Text_ActionButton->SetText(FText::FromString(TEXT("Buy")));
		break;

	case EShopTab::Sell:
		Text_ActionButton->SetText(FText::FromString(TEXT("Sell")));
		break;

	case EShopTab::ReBuy:
		Text_ActionButton->SetText(FText::FromString(TEXT("ReBuy")));
		break;

	default:
		Text_ActionButton->SetText(FText::FromString(TEXT("Buy")));
		break;
	}
}

void UWidget_Shop::ClearItemGrid()
{
	if (ItemGridPanel)
	{
		ItemGridPanel->ClearChildren();
	}
}

void UWidget_Shop::ShowEmptyMessage(const FText& Message)
{
	if (Text_EmptyMessage)
	{
		Text_EmptyMessage->SetText(Message);
		Text_EmptyMessage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

UMuksiItemDataAsset* UWidget_Shop::ResolveShopItemData(const FShopItemEntry& Entry) const
{
	UMuksiItemDataSubsystem* ItemDataSubsystem = UMuksiItemDataSubsystem::Get(this);
	return ItemDataSubsystem ? ItemDataSubsystem->FindItemData(Entry.ItemID) : nullptr;
}