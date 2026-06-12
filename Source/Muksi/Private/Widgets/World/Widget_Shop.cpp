#include "Widgets/World/Widget_Shop.h"

#include "Widgets/World/Widget_ShopItemEntry.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Items/Components/InventoryComponent.h"

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

	RefreshShop();
	RefreshSelection();
}

void UWidget_Shop::SetShopData(UShopDataAsset* InShopData)
{
	ShopData = InShopData;

	if (IsActivated())
	{
		RefreshShop();
		RefreshSelection();
	}
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
		if (!Entry.ItemData)
		{
			continue;
		}

		UWidget_ShopItemEntry* EntryWidget =
			CreateWidget<UWidget_ShopItemEntry>(GetOwningPlayer(), ShopItemEntryClass);

		if (!EntryWidget)
		{
			continue;
		}

		EntryWidget->Setup(Entry);
		EntryWidget->SetSelected(
			bHasSelectedItem &&
			SelectedShopItem.ItemData == Entry.ItemData
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
	UE_LOG(LogTemp, Log, TEXT("[Shop] SelectShopItem Item=%s Price=%d"),
		Entry.ItemData ? *Entry.ItemData->DisplayName.ToString() : TEXT("None"),
		Entry.Price);

	SelectedShopItem = Entry;
	bHasSelectedItem = true;

	RefreshShop();
	RefreshSelection();
}

void UWidget_Shop::RefreshSelection()
{
	const bool bCanBuy = CanBuySelectedItem();

	if (Text_CurrentGold)
	{
		Text_CurrentGold->SetText(FText::AsNumber(CurrentGold));
	}

	if (!bHasSelectedItem || !SelectedShopItem.ItemData)
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
		Text_SelectedItemName->SetText(SelectedShopItem.ItemData->DisplayName);
	}

	if (Text_SelectedItemDescription)
	{
		Text_SelectedItemDescription->SetText(SelectedShopItem.ItemData->Description);
	}

	if (Text_Price)
	{
		Text_Price->SetText(FText::AsNumber(SelectedShopItem.Price));
	}

	if (Text_AfterGold)
	{
		Text_AfterGold->SetText(FText::AsNumber(CurrentGold - SelectedShopItem.Price));
	}

	if (Button_Buy)
	{
		Button_Buy->SetIsEnabled(bCanBuy);
	}
}

bool UWidget_Shop::CanBuySelectedItem() const
{
	if (CurrentTab != EShopTab::Buy)
	{
		return false;
	}

	if (!bHasSelectedItem || !SelectedShopItem.ItemData)
	{
		return false;
	}

	if (SelectedShopItem.Price > CurrentGold)
	{
		return false;
	}

	if (!SelectedShopItem.bUnlimitedStock && SelectedShopItem.Stock <= 0)
	{
		return false;
	}

	return true;
}

bool UWidget_Shop::TryBuySelectedItem()
{
	if (!CanBuySelectedItem())
	{
		UE_LOG(LogTemp, Warning, TEXT("Shop buy failed."));
		return false;
	}

	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shop buy failed: InventoryComponent not found."));
		return false;
	}

	InventoryComponent->AddItem(SelectedShopItem.ItemData, 1);

	CurrentGold -= SelectedShopItem.Price;


	return true;
}

void UWidget_Shop::HandleBuyClicked()
{
	if (TryBuySelectedItem())
	{
		RefreshShop();
		RefreshSelection();
	}
}

void UWidget_Shop::HandleCloseClicked()
{
	DeactivateWidget();
}

UInventoryComponent* UWidget_Shop::GetInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;

	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
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