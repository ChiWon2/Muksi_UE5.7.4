#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_Shop.h"

#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopItemEntry.h"
#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopInventoryPanel.h"

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

	if (Button_Buy)
	{
		Button_Buy->OnClicked.RemoveAll(this);
		Button_Buy->OnClicked.AddDynamic(this, &ThisClass::HandleBuyClicked);
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
}

void UWidget_Shop::NativeOnActivated()
{
	Super::NativeOnActivated();

	UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this);

	UInventoryComponent* InventoryComponent =
		PlayerData ? PlayerData->GetPlayerInventoryComponent() : nullptr;

	UEquipmentComponent* EquipmentComponent =
		PlayerData ? PlayerData->GetPlayerEquipmentComponent() : nullptr;

	UPlayerCurrencyComponent* CurrencyComponent =
		PlayerData ? PlayerData->GetPlayerCurrencyComponent() : nullptr;

	UE_LOG(LogTemp, Warning,
		TEXT("[Widget_Shop] PlayerData=%s Inventory=%s Equipment=%s Currency=%s Owner=%s"),
		*GetNameSafe(PlayerData),
		*GetNameSafe(InventoryComponent),
		*GetNameSafe(EquipmentComponent),
		*GetNameSafe(CurrencyComponent),
		CurrencyComponent ? *GetNameSafe(CurrencyComponent->GetOwner()) : TEXT("None"));

	if (CurrencyComponent)
	{
		CurrencyComponent->OnGoldChanged.RemoveAll(this);
		CurrencyComponent->OnGoldChanged.AddDynamic(
			this, &ThisClass::HandleGoldChanged);
	}

	RefreshShop();
	RefreshSelection();

	if (ShopInventoryPanel)
	{
		ShopInventoryPanel->Refresh();
	}
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
		RefreshShop();
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

void UWidget_Shop::RefreshShop()
{
	if (!ItemGridPanel)
	{
		return;
	}

	if (CurrentTab != EShopTab::Buy)
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
			UE_LOG(LogTemp, Warning,
				TEXT("[Widget_Shop] Shop item skipped. ItemID=%s"),
				*Entry.ItemID.ToString());
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
			SelectedShopItem.ShopEntryId == Entry.ShopEntryId
		);
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

	RefreshShop();
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

		if (Button_Buy)
		{
			Button_Buy->SetIsEnabled(false);
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

	if (Button_Buy)
	{
		Button_Buy->SetIsEnabled(bCanBuy);
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

void UWidget_Shop::HandleBuyClicked()
{
	if (!TryBuySelectedItem())
	{
		RefreshSelection();
		return;
	}

	RefreshShop();
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

UInventoryComponent* UWidget_Shop::GetInventoryComponent() const
{
	if (UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this))
	{
		return PlayerData->GetPlayerInventoryComponent();
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
		return;
	}

	CurrentTab = NewTab;

	bHasSelectedItem = false;
	SelectedShopItem = FShopItemEntry();

	RefreshTabContent();
	RefreshSelection();
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
		RefreshShop();
		break;

	case EShopTab::Sell:
		ClearItemGrid();
		ShowEmptyMessage(FText::FromString(TEXT("판매 기능 준비중")));
		UE_LOG(LogTemp, Log, TEXT("[Shop] Sell tab clicked. Sell feature is not implemented yet."));
		break;

	case EShopTab::ReBuy:
		ClearItemGrid();
		ShowEmptyMessage(FText::FromString(TEXT("재구매 기능 준비중")));
		UE_LOG(LogTemp, Log, TEXT("[Shop] ReBuy tab clicked. ReBuy feature is not implemented yet."));
		break;

	default:
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