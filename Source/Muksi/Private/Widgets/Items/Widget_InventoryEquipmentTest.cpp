#include "Widgets/Items/Widget_InventoryEquipmentTest.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/Pawn.h"
#include "Items/Components/EquipmentComponent.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Components/StatComponent.h"

void UWidget_InventoryEquipmentTest::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (AddTestItemButton)
	{
		AddTestItemButton->OnClicked.RemoveAll(this);
		AddTestItemButton->OnClicked.AddDynamic(this, &ThisClass::HandleAddTestItemButtonClicked);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.RemoveAll(this);
		EquipButton->OnClicked.AddDynamic(this, &ThisClass::HandleEquipButtonClicked);
	}

	if (UnequipWeaponButton)
	{
		UnequipWeaponButton->OnClicked.RemoveAll(this);
		UnequipWeaponButton->OnClicked.AddDynamic(this, &ThisClass::HandleUnequipWeaponButtonClicked);
	}

	if (UnequipArmorButton)
	{
		UnequipArmorButton->OnClicked.RemoveAll(this);
		UnequipArmorButton->OnClicked.AddDynamic(this, &ThisClass::HandleUnequipArmorButtonClicked);
	}

	if (UnequipAccessoryButton)
	{
		UnequipAccessoryButton->OnClicked.RemoveAll(this);
		UnequipAccessoryButton->OnClicked.AddDynamic(this, &ThisClass::HandleUnequipAccessoryButtonClicked);
	}

	if (RefreshButton)
	{
		RefreshButton->OnClicked.RemoveAll(this);
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::HandleRefreshButtonClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &ThisClass::HandleBackButtonClicked);
		BackButton->SetFocus();
	}

	RefreshAll();
}

void UWidget_InventoryEquipmentTest::HandleAddTestItemButtonClicked()
{
	UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryEquipmentTest] Add failed: InventoryComponent is null"));
		return;
	}

	if (TestItemDataAssets.IsEmpty() || !TestItemDataAssets[0])
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryEquipmentTest] Add failed: TestItemDataAssets is empty"));
		return;
	}

	InventoryComponent->AddItem(TestItemDataAssets[0], 1);
	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipmentTest] Test item added: %s"), *GetNameSafe(TestItemDataAssets[0]));

	RefreshAll();
}

void UWidget_InventoryEquipmentTest::HandleEquipButtonClicked()
{
	UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent || !SelectedInstanceId.IsValid())
	{
		return;
	}

	const bool bResult = EquipmentComponent->EquipItemByInstanceId(SelectedInstanceId);
	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipmentTest] Equip result: %s"), bResult ? TEXT("true") : TEXT("false"));

	RefreshAll();
}

void UWidget_InventoryEquipmentTest::HandleUnequipWeaponButtonClicked()
{
	UnequipSlot(EMuksiEquipmentSlot::Weapon);
}

void UWidget_InventoryEquipmentTest::HandleUnequipArmorButtonClicked()
{
	UnequipSlot(EMuksiEquipmentSlot::Armor);
}

void UWidget_InventoryEquipmentTest::HandleUnequipAccessoryButtonClicked()
{
	UnequipSlot(EMuksiEquipmentSlot::Accessory);
}

void UWidget_InventoryEquipmentTest::HandleRefreshButtonClicked()
{
	RefreshAll();
}

void UWidget_InventoryEquipmentTest::HandleBackButtonClicked()
{
	DeactivateWidget();
}

void UWidget_InventoryEquipmentTest::RefreshAll()
{
	RefreshItemList();
	RefreshSelectedText();
	RefreshEquippedText();
	RefreshStatPreviewText();
}

void UWidget_InventoryEquipmentTest::RefreshItemList()
{
	if (!ItemListBox)
	{
		return;
	}

	ItemListBox->ClearChildren();
	ButtonToInstanceIdMap.Empty();

	const UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();
	if (!InventoryComponent)
	{
		return;
	}

	for (const FMuksiInventoryEntry& Entry : InventoryComponent->GetItems())
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		UButton* ItemButton = NewObject<UButton>(this);
		UTextBlock* ItemText = NewObject<UTextBlock>(ItemButton);

		const FString Line = FString::Printf(
			TEXT("%s x%d | %s"),
			*Entry.ItemData->DisplayName.ToString(),
			Entry.Quantity,
			*Entry.InstanceId.ToString(EGuidFormats::DigitsWithHyphens)
		);

		ItemText->SetText(FText::FromString(Line));
		ItemButton->AddChild(ItemText);
		ButtonToInstanceIdMap.Add(ItemButton, Entry.InstanceId);
		ItemButton->OnClicked.AddDynamic(this, &ThisClass::HandleDynamicItemButtonClicked);

		ItemListBox->AddChild(ItemButton);
	}
}

void UWidget_InventoryEquipmentTest::RefreshSelectedText()
{
	if (!SelectedItemText)
	{
		return;
	}

	if (!SelectedInstanceId.IsValid())
	{
		SelectedItemText->SetText(FText::FromString(TEXT("Selected: None")));
		return;
	}

	SelectedItemText->SetText(FText::FromString(
		FString::Printf(TEXT("Selected: %s"), *SelectedInstanceId.ToString(EGuidFormats::DigitsWithHyphens))
	));
}

void UWidget_InventoryEquipmentTest::RefreshEquippedText()
{
	if (!EquippedItemText)
	{
		return;
	}

	const UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent)
	{
		EquippedItemText->SetText(FText::FromString(TEXT("EquipmentComponent: None")));
		return;
	}

	auto GetSlotText = [EquipmentComponent](EMuksiEquipmentSlot EquipmentSlot, const TCHAR* SlotName)
		{
			FMuksiInventoryEntry Entry;
			if (EquipmentComponent->GetEquippedItem(EquipmentSlot, Entry) && Entry.ItemData)
			{
				return FString::Printf(TEXT("%s: %s"), SlotName, *Entry.ItemData->DisplayName.ToString());
			}

			return FString::Printf(TEXT("%s: None"), SlotName);
		};

	const FString Text = FString::Printf(
		TEXT("%s\n%s\n%s"),
		*GetSlotText(EMuksiEquipmentSlot::Weapon, TEXT("Weapon")),
		*GetSlotText(EMuksiEquipmentSlot::Armor, TEXT("Armor")),
		*GetSlotText(EMuksiEquipmentSlot::Accessory, TEXT("Accessory"))
	);

	EquippedItemText->SetText(FText::FromString(Text));
}

void UWidget_InventoryEquipmentTest::RefreshStatPreviewText()
{
	if (!StatPreviewText)
	{
		return;
	}

	const UStatComponent* StatComponent = GetPlayerStatComponent();
	if (!StatComponent)
	{
		StatPreviewText->SetText(FText::FromString(TEXT("StatComponent: None")));
		return;
	}

	const FString Text = FString::Printf(
		TEXT("HP %.0f / %.0f\nAttack %.0f\nDefense %.0f\nMoveSpeed %.0f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower(),
		StatComponent->GetMoveSpeed()
	);

	StatPreviewText->SetText(FText::FromString(Text));
}

void UWidget_InventoryEquipmentTest::SelectItem(FGuid InstanceId)
{
	SelectedInstanceId = InstanceId;
	RefreshSelectedText();

	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipmentTest] Selected item: %s"),
		*SelectedInstanceId.ToString(EGuidFormats::DigitsWithHyphens));
}

void UWidget_InventoryEquipmentTest::UnequipSlot(EMuksiEquipmentSlot EquipmentSlot)
{
	UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent)
	{
		return;
	}

	const bool bResult = EquipmentComponent->UnequipItem(EquipmentSlot);
	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipmentTest] Unequip result: %s"), bResult ? TEXT("true") : TEXT("false"));

	RefreshAll();
}

UInventoryComponent* UWidget_InventoryEquipmentTest::GetPlayerInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}

UEquipmentComponent* UWidget_InventoryEquipmentTest::GetPlayerEquipmentComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UEquipmentComponent>() : nullptr;
}

UStatComponent* UWidget_InventoryEquipmentTest::GetPlayerStatComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UStatComponent>() : nullptr;
}

void UWidget_InventoryEquipmentTest::HandleDynamicItemButtonClicked()
{
	for (const TPair<TObjectPtr<UButton>, FGuid>& Pair : ButtonToInstanceIdMap)
	{
		if (!Pair.Key)
		{
			continue;
		}

		if (Pair.Key->HasUserFocus(GetOwningPlayer()))
		{
			SelectItem(Pair.Value);
			return;
		}
	}
}