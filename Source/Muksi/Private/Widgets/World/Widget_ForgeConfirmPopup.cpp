#include "Widgets/World/Widget_ForgeConfirmPopup.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_ForgeConfirmPopup::InitializeForgePopup(
	FGuid InInstanceId,
	EMuksiForgeActionType InActionType)
{
	InstanceId = InInstanceId;
	ActionType = InActionType;

	RefreshInfo();
}

void UWidget_ForgeConfirmPopup::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.RemoveAll(this);
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::HandleConfirmButtonClicked);
		ConfirmButton->SetFocus();
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.RemoveAll(this);
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::HandleCancelButtonClicked);
	}

	RefreshInfo();
}

void UWidget_ForgeConfirmPopup::RefreshInfo()
{
	const UInventoryComponent* InventoryComponent = GetInventoryComponent();

	FMuksiInventoryEntry Entry;
	if (!InventoryComponent || !InventoryComponent->FindItemByInstanceId(InstanceId, Entry) || !Entry.ItemData)
	{
		if (ConfirmButton)
		{
			ConfirmButton->SetIsEnabled(false);
		}

		if (ConfirmButtonText)
		{
			ConfirmButtonText->SetText(GetConfirmButtonText());
		}

		return;
	}

	if (TitleText)
	{
		TitleText->SetText(GetActionTitleText());
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

	if (EnhanceLevelText)
	{
		if (ActionType == EMuksiForgeActionType::Enhance)
		{
			EnhanceLevelText->SetText(FText::FromString(
				FString::Printf(TEXT("Enhance: +%d -> +%d"), Entry.EnhanceLevel, Entry.EnhanceLevel + 1)
			));
		}
		else
		{
			EnhanceLevelText->SetText(FText::FromString(
				FString::Printf(TEXT("Enhance: +%d"), Entry.EnhanceLevel)
			));
		}
	}

	if (ResultText)
	{
		ResultText->SetText(FText::GetEmpty());
		ResultText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ActionType == EMuksiForgeActionType::Repair)
	{
		RefreshRepairInfo(Entry, InventoryComponent);
	}
	else if (ActionType == EMuksiForgeActionType::Enhance)
	{
		RefreshEnhanceInfo(Entry, InventoryComponent);
	}
}

void UWidget_ForgeConfirmPopup::RefreshRepairInfo(
	const FMuksiInventoryEntry& Entry,
	const UInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return;
	}

	if (CostText)
	{
		CostText->SetText(FText::FromString(
			FString::Printf(TEXT("Repair Cost: %d"), InventoryComponent->GetRepairCost(InstanceId))
		));
	}

	if (SuccessRateText)
	{
		SuccessRateText->SetText(FText::GetEmpty());
		SuccessRateText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (FailureInfoText)
	{
		FailureInfoText->SetText(FText::GetEmpty());
		FailureInfoText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(InventoryComponent->IsRepairableItem(InstanceId));
	}
}

void UWidget_ForgeConfirmPopup::RefreshEnhanceInfo(
	const FMuksiInventoryEntry& Entry,
	const UInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return;
	}

	if (CostText)
	{
		CostText->SetText(FText::FromString(
			FString::Printf(TEXT("Enhance Cost: %d"), InventoryComponent->GetEnhanceCost(InstanceId))
		));
	}

	if (SuccessRateText)
	{
		const int32 SuccessPercent = FMath::RoundToInt(InventoryComponent->GetEnhanceSuccessRate(InstanceId) * 100.f);

		SuccessRateText->SetVisibility(ESlateVisibility::Visible);
		SuccessRateText->SetText(FText::FromString(
			FString::Printf(TEXT("Success Rate: %d%%"), SuccessPercent)
		));
	}

	if (FailureInfoText)
	{
		FailureInfoText->SetVisibility(ESlateVisibility::Visible);

		const FString FailureInfo = Entry.EnhanceLevel >= 6
			? TEXT("Failure: durability decreases and enhance level may drop.")
			: TEXT("Failure: durability decreases.");

		FailureInfoText->SetText(FText::FromString(FailureInfo));
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(InventoryComponent->IsEnhanceableItem(InstanceId));
	}
}

void UWidget_ForgeConfirmPopup::HandleConfirmButtonClicked()
{
	UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent || !InstanceId.IsValid())
	{
		return;
	}

	if (ActionType == EMuksiForgeActionType::Repair)
	{
		int32 RepairCost = 0;
		if (InventoryComponent->RepairItem(InstanceId, RepairCost))
		{
			OnForgeActionCompleted.Broadcast(InstanceId, ActionType);
			DeactivateWidget();
		}

		return;
	}

	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		int32 EnhanceCost = 0;
		EMuksiEnhanceResult EnhanceResult = EMuksiEnhanceResult::None;

		if (InventoryComponent->EnhanceItem(InstanceId, EnhanceCost, EnhanceResult))
		{
			if (ResultText)
			{
				ResultText->SetVisibility(ESlateVisibility::Visible);
				ResultText->SetText(GetEnhanceResultText(EnhanceResult));
			}

			OnForgeActionCompleted.Broadcast(InstanceId, ActionType);

			// 결과를 보여주고 싶으면 닫지 않고 RefreshInfo만 호출.
			// 바로 닫는 Repair와 같은 흐름을 원하면 아래 DeactivateWidget 사용.
			RefreshInfo();
		}
	}
}

void UWidget_ForgeConfirmPopup::HandleCancelButtonClicked()
{
	DeactivateWidget();
}

FText UWidget_ForgeConfirmPopup::GetActionTitleText() const
{
	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		return FText::FromString(TEXT("Enhance Item"));
	}

	return FText::FromString(TEXT("Repair Item"));
}

FText UWidget_ForgeConfirmPopup::GetConfirmButtonText() const
{
	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		return FText::FromString(TEXT("Enhance"));
	}

	return FText::FromString(TEXT("Repair"));
}

FText UWidget_ForgeConfirmPopup::GetEnhanceResultText(EMuksiEnhanceResult Result) const
{
	switch (Result)
	{
	case EMuksiEnhanceResult::Success:
		return FText::FromString(TEXT("Enhance succeeded."));
	case EMuksiEnhanceResult::FailedNoChange:
		return FText::FromString(TEXT("Enhance failed. Durability decreased."));
	case EMuksiEnhanceResult::FailedDowngrade:
		return FText::FromString(TEXT("Enhance failed. Level and durability decreased."));
	case EMuksiEnhanceResult::FailedBroken:
		return FText::FromString(TEXT("Enhance failed. Item was broken."));
	default:
		return FText::GetEmpty();
	}
}

UInventoryComponent* UWidget_ForgeConfirmPopup::GetInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}