#include "Widgets/World/Widget_RepairConfirmPopup.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_RepairConfirmPopup::InitializeRepairPopup(FGuid InInstanceId)
{
	InstanceId = InInstanceId;
	RefreshInfo();
}

void UWidget_RepairConfirmPopup::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (RepairButton)
	{
		RepairButton->OnClicked.RemoveAll(this);
		RepairButton->OnClicked.AddDynamic(this, &ThisClass::HandleRepairButtonClicked);
		RepairButton->SetFocus();
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.RemoveAll(this);
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::HandleCancelButtonClicked);
	}

	RefreshInfo();
}

void UWidget_RepairConfirmPopup::RefreshInfo()
{
	const UInventoryComponent* InventoryComponent = GetInventoryComponent();

	FMuksiInventoryEntry Entry;
	if (!InventoryComponent || !InventoryComponent->FindItemByInstanceId(InstanceId, Entry) || !Entry.ItemData)
	{
		if (RepairButton)
		{
			RepairButton->SetIsEnabled(false);
		}
		return;
	}

	if (ItemNameText)
	{
		const FString Name = Entry.EnhanceLevel > 0
			? FString::Printf(TEXT("%s +%d"), *Entry.ItemData->DisplayName.ToString(), Entry.EnhanceLevel)
			: Entry.ItemData->DisplayName.ToString();

		ItemNameText->SetText(FText::FromString(Name));
	}

	if (DurabilityText)
	{
		const int32 Percent = FMath::RoundToInt(FMath::Clamp(Entry.Durability, 0.f, 1.f) * 100.f);
		DurabilityText->SetText(FText::FromString(FString::Printf(TEXT("Durability: %d%%"), Percent)));
	}

	if (RepairCostText)
	{
		RepairCostText->SetText(FText::FromString(
			FString::Printf(TEXT("Repair Cost: %d"), InventoryComponent->GetRepairCost(InstanceId))
		));
	}

	if (RepairButton)
	{
		RepairButton->SetIsEnabled(InventoryComponent->IsRepairableItem(InstanceId));
	}
}

void UWidget_RepairConfirmPopup::HandleRepairButtonClicked()
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent || !InstanceId.IsValid())
	{
		return;
	}

	int32 RepairCost = 0;
	if (InventoryComponent->RepairItem(InstanceId, RepairCost))
	{
		OnRepairCompleted.Broadcast(InstanceId);
		DeactivateWidget();
	}
}

void UWidget_RepairConfirmPopup::HandleCancelButtonClicked()
{
	DeactivateWidget();
}

UInventoryComponent* UWidget_RepairConfirmPopup::GetInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}