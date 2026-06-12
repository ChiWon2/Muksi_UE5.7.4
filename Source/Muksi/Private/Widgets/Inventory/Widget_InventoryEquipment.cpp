#include "Widgets/Inventory/Widget_InventoryEquipment.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Input/Reply.h"
#include "Items/Components/EquipmentComponent.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"
#include "Widgets/Inventory/Widget_CharacterInfoPanel.h"
#include "Widgets/Inventory/Widget_EquipmentSlot.h"
#include "Widgets/Inventory/Widget_ItemSlot.h"

void UWidget_InventoryEquipment::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &ThisClass::HandleBackClicked);
		BackButton->SetFocus();
	}

	if (EquipmentCategoryButton)
	{
		EquipmentCategoryButton->OnClicked.RemoveAll(this);
		EquipmentCategoryButton->OnClicked.AddDynamic(this, &ThisClass::HandleEquipmentCategoryClicked);
	}

	if (ConsumableCategoryButton)
	{
		ConsumableCategoryButton->OnClicked.RemoveAll(this);
		ConsumableCategoryButton->OnClicked.AddDynamic(this, &ThisClass::HandleConsumableCategoryClicked);
	}

	if (SpecialCategoryButton)
	{
		SpecialCategoryButton->OnClicked.RemoveAll(this);
		SpecialCategoryButton->OnClicked.AddDynamic(this, &ThisClass::HandleSpecialCategoryClicked);
	}

	if (CharacterInfoButton)
	{
		CharacterInfoButton->OnClicked.RemoveAll(this);
		CharacterInfoButton->OnClicked.AddDynamic(this, &ThisClass::HandleCharacterInfoButtonClicked);
	}

	if (CharacterInfoPanel)
	{
		CharacterInfoPanel->SetVisibility(ESlateVisibility::Visible);
		CharacterInfoPanel->SetShowDetailButton(true);
		CharacterInfoPanel->SetDisplayMode(ECharacterInfoDisplayMode::Compact);
	}

	RefreshAll();
}

void UWidget_InventoryEquipment::NativeOnDeactivated()
{
	SelectedInstanceId.Invalidate();
	SelectedEquipmentSlot = EMuksiEquipmentSlot::None;
	bDraggingWindow = false;

	Super::NativeOnDeactivated();
}

FReply UWidget_InventoryEquipment::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (TitleBar && WindowRoot && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();

		if (TitleBar->GetCachedGeometry().IsUnderLocation(ScreenPosition))
		{
			bDraggingWindow = true;
			DragOffset = WindowRoot->GetCachedGeometry().AbsoluteToLocal(ScreenPosition);

			return FReply::Handled().CaptureMouse(TakeWidget());
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UWidget_InventoryEquipment::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDraggingWindow && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDraggingWindow = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UWidget_InventoryEquipment::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDraggingWindow && WindowRoot)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(WindowRoot->Slot);
		if (CanvasSlot)
		{
			const FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			CanvasSlot->SetPosition(LocalMousePosition - DragOffset);

			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UWidget_InventoryEquipment::HandleBackClicked()
{
	DeactivateWidget();
}

void UWidget_InventoryEquipment::HandleEquipmentCategoryClicked()
{
	SetCategoryFilter(EInventoryCategoryFilter::Equipment);
}

void UWidget_InventoryEquipment::HandleConsumableCategoryClicked()
{
	SetCategoryFilter(EInventoryCategoryFilter::Consumable);
}

void UWidget_InventoryEquipment::HandleSpecialCategoryClicked()
{
	SetCategoryFilter(EInventoryCategoryFilter::Special);
}

void UWidget_InventoryEquipment::HandleItemSlotClicked(FGuid InstanceId)
{
	SelectedInstanceId = InstanceId;
	SelectedEquipmentSlot = EMuksiEquipmentSlot::None;
}

void UWidget_InventoryEquipment::HandleItemSlotRightClicked(FGuid InstanceId)
{
	UEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!EquipmentComponent || !InstanceId.IsValid())
	{
		return;
	}

	if (EquipmentComponent->EquipItemByInstanceId(InstanceId))
	{
		SelectedInstanceId = InstanceId;
		SelectedEquipmentSlot = EMuksiEquipmentSlot::None;

		RefreshInventoryGrid();
		RefreshEquipmentSlots();
		RefreshCharacterInfo();
	}
}

void UWidget_InventoryEquipment::HandleEquipmentSlotClicked(EMuksiEquipmentSlot EquipmentSlot)
{
	SelectedInstanceId.Invalidate();
	SelectedEquipmentSlot = EquipmentSlot;
}

void UWidget_InventoryEquipment::HandleEquipmentSlotRightClicked(EMuksiEquipmentSlot EquipmentSlot)
{
	UEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!EquipmentComponent || EquipmentSlot == EMuksiEquipmentSlot::None)
	{
		return;
	}

	if (EquipmentComponent->UnequipItem(EquipmentSlot))
	{
		SelectedInstanceId.Invalidate();
		SelectedEquipmentSlot = EquipmentSlot;

		RefreshInventoryGrid();
		RefreshEquipmentSlots();
		RefreshCharacterInfo();
	}
}

void UWidget_InventoryEquipment::HandleCharacterInfoButtonClicked()
{
	if (!CharacterInfoPanel)
	{
		return;
	}

	CharacterInfoPanel->ToggleDisplayMode();
}

void UWidget_InventoryEquipment::SetCategoryFilter(EInventoryCategoryFilter NewFilter)
{
	if (CurrentCategoryFilter == NewFilter)
	{
		return;
	}

	CurrentCategoryFilter = NewFilter;
	RefreshInventoryGrid();
}

void UWidget_InventoryEquipment::RefreshAll()
{
	RefreshInventoryGrid();
	RefreshEquipmentSlots();
	RefreshCharacterInfo();
}

void UWidget_InventoryEquipment::RefreshInventoryGrid()
{
	if (!InventoryGridPanel)
	{
		return;
	}

	InventoryGridPanel->ClearChildren();

	const UInventoryComponent* InventoryComponent = GetInventoryComponent();
	const UEquipmentComponent* EquipmentComponent = GetEquipmentComponent();

	if (!InventoryComponent || !ItemSlotClass)
	{
		return;
	}

	const int32 SafeColumnCount = FMath::Max(1, InventoryColumnCount);
	int32 VisibleIndex = 0;

	for (const FMuksiInventoryEntry& Entry : InventoryComponent->GetItems())
	{
		if (!DoesItemPassCategoryFilter(Entry))
		{
			continue;
		}

		UWidget_ItemSlot* SlotWidget = CreateWidget<UWidget_ItemSlot>(GetOwningPlayer(), ItemSlotClass);
		if (!SlotWidget)
		{
			continue;
		}

		const bool bEquipped = EquipmentComponent && EquipmentComponent->IsEquipped(Entry.InstanceId);

		SlotWidget->InitializeSlot(Entry, bEquipped);
		SlotWidget->SetSelected(Entry.InstanceId == SelectedInstanceId);
		SlotWidget->OnClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
		SlotWidget->OnRightClicked.AddDynamic(this, &ThisClass::HandleItemSlotRightClicked);

		const int32 Row = VisibleIndex / SafeColumnCount;
		const int32 Column = VisibleIndex % SafeColumnCount;

		InventoryGridPanel->AddChildToUniformGrid(SlotWidget, Row, Column);

		++VisibleIndex;
	}
}

void UWidget_InventoryEquipment::RefreshEquipmentSlots()
{
	if (!EquipmentSlotBox)
	{
		return;
	}

	EquipmentSlotBox->ClearChildren();

	AddEquipmentSlot(EMuksiEquipmentSlot::Weapon);
	AddEquipmentSlot(EMuksiEquipmentSlot::Armor);
	AddEquipmentSlot(EMuksiEquipmentSlot::Accessory);
}

void UWidget_InventoryEquipment::RefreshCharacterInfo()
{
	if (CharacterInfoPanel)
	{
		CharacterInfoPanel->RefreshCharacterInfo();
	}
}

void UWidget_InventoryEquipment::AddEquipmentSlot(EMuksiEquipmentSlot EquipmentSlot)
{
	if (!EquipmentSlotBox || !EquipmentSlotClass)
	{
		return;
	}

	const UEquipmentComponent* EquipmentComponent = GetEquipmentComponent();

	FMuksiInventoryEntry EquippedEntry;
	const bool bHasEquippedItem =
		EquipmentComponent &&
		EquipmentComponent->GetEquippedItem(EquipmentSlot, EquippedEntry) &&
		EquippedEntry.ItemData;

	UWidget_EquipmentSlot* SlotWidget = CreateWidget<UWidget_EquipmentSlot>(GetOwningPlayer(), EquipmentSlotClass);
	if (!SlotWidget)
	{
		return;
	}

	SlotWidget->InitializeSlot(EquipmentSlot, EquippedEntry, bHasEquippedItem);
	SlotWidget->SetSelected(SelectedEquipmentSlot == EquipmentSlot);
	SlotWidget->OnClicked.AddDynamic(this, &ThisClass::HandleEquipmentSlotClicked);
	SlotWidget->OnRightClicked.AddDynamic(this, &ThisClass::HandleEquipmentSlotRightClicked);

	EquipmentSlotBox->AddChild(SlotWidget);
}

bool UWidget_InventoryEquipment::DoesItemPassCategoryFilter(const FMuksiInventoryEntry& Entry) const
{
	if (!Entry.ItemData)
	{
		return false;
	}

	switch (CurrentCategoryFilter)
	{
	case EInventoryCategoryFilter::Equipment:
		return Entry.ItemData->ItemType == EMuksiItemType::Equipment;

	case EInventoryCategoryFilter::Consumable:
		return Entry.ItemData->ItemType == EMuksiItemType::Consumable;

	case EInventoryCategoryFilter::Special:
		return Entry.ItemData->ItemType == EMuksiItemType::Material ||
			Entry.ItemData->ItemType == EMuksiItemType::Quest;

	default:
		return false;
	}
}

UMuksiPlayerDataSubsystem* UWidget_InventoryEquipment::GetPlayerDataSubsystem() const
{
	return UMuksiPlayerDataSubsystem::Get(this);
}

UInventoryComponent* UWidget_InventoryEquipment::GetInventoryComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerDataSubsystem = GetPlayerDataSubsystem();
	return PlayerDataSubsystem ? PlayerDataSubsystem->GetPlayerInventoryComponent() : nullptr;
}

UEquipmentComponent* UWidget_InventoryEquipment::GetEquipmentComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerDataSubsystem = GetPlayerDataSubsystem();
	return PlayerDataSubsystem ? PlayerDataSubsystem->GetPlayerEquipmentComponent() : nullptr;
}
