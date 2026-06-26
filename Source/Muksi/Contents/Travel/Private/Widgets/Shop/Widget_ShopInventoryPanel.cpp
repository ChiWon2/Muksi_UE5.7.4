#include "Muksi/Contents/Travel/Public/Widgets/Shop/Widget_ShopInventoryPanel.h"

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

void UWidget_ShopInventoryPanel::RefreshInventory()
{
	if (!InventoryGridPanel)
	{
		return;
	}

	InventoryGridPanel->ClearChildren();

	const UInventoryComponent* InventoryComponent =
		GetInventoryComponent();

	if (!InventoryComponent || !ItemSlotClass)
	{
		return;
	}

	const int32 SafeColumnCount =
		FMath::Max(1, InventoryColumnCount);

	int32 VisibleIndex = 0;

	for (const FMuksiInventoryEntry& Entry :
		InventoryComponent->GetItems())
	{
		if (!Entry.ItemData || !Entry.InstanceId.IsValid())
		{
			continue;
		}

		UWidget_ItemSlot* SlotWidget =
			CreateWidget<UWidget_ItemSlot>(
				GetOwningPlayer(),
				ItemSlotClass);

		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->InitializeSlot(Entry, false);
		SlotWidget->SetSelected(false);

		const int32 Row = VisibleIndex / SafeColumnCount;
		const int32 Column = VisibleIndex % SafeColumnCount;

		InventoryGridPanel->AddChildToUniformGrid(
			SlotWidget,
			Row,
			Column);

		++VisibleIndex;
	}
}

void UWidget_ShopInventoryPanel::HandleGoldChanged(int32 NewGold)
{
	if (Text_Gold)
	{
		Text_Gold->SetText(FText::AsNumber(NewGold));
	}
}

UInventoryComponent*
UWidget_ShopInventoryPanel::GetInventoryComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this);

	return PlayerData
		? PlayerData->GetPlayerInventoryComponent()
		: nullptr;
}

UPlayerCurrencyComponent*
UWidget_ShopInventoryPanel::GetCurrencyComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerData =
		UMuksiPlayerDataSubsystem::Get(this);

	return PlayerData
		? PlayerData->GetPlayerCurrencyComponent()
		: nullptr;
}