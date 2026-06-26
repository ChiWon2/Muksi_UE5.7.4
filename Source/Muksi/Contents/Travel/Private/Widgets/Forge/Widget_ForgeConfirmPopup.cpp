#include "Muksi/Contents/Travel/Public/Widgets/Forge/Widget_ForgeConfirmPopup.h"

#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"
#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"

void UWidget_ForgeConfirmPopup::InitializeForgePopup(FGuid InInstanceId, EMuksiForgeActionType InActionType)
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

void UWidget_ForgeConfirmPopup::RefreshRepairInfo(const FMuksiInventoryEntry& Entry, const UInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return;
	}

	const int32 RepairCost = InventoryComponent->GetRepairCost(InstanceId);
	const bool bCanRepair = InventoryComponent->IsRepairableItem(InstanceId);
	const bool bCanAfford = CanAffordCurrentAction(InventoryComponent);

	if (CostText)
	{
		CostText->SetText(FText::FromString(
			FString::Printf(TEXT("Repair Cost: %d"), RepairCost)
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

	RefreshGoldInfo(InventoryComponent);

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bCanRepair && bCanAfford);
	}
}

void UWidget_ForgeConfirmPopup::RefreshEnhanceInfo(const FMuksiInventoryEntry& Entry, const UInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return;
	}

	const int32 EnhanceCost = InventoryComponent->GetEnhanceCost(InstanceId);
	const bool bCanEnhance = InventoryComponent->IsEnhanceableItem(InstanceId);
	const bool bCanAfford = CanAffordCurrentAction(InventoryComponent);

	if (CostText)
	{
		CostText->SetText(FText::FromString(
			FString::Printf(TEXT("Enhance Cost: %d"), EnhanceCost)
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

	RefreshGoldInfo(InventoryComponent);

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bCanEnhance && bCanAfford);
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
		const int32 RepairCost = InventoryComponent->GetRepairCost(InstanceId);
		UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();

		if (RepairCost <= 0 || !CurrencyComponent || !CurrencyComponent->HasEnoughGold(RepairCost))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ForgeCurrency] Repair failed. Not enough Gold. Cost=%d"), RepairCost);
			RefreshInfo();
			return;
		}

		int32 OutRepairCost = 0;
		if (InventoryComponent->RepairItem(InstanceId, OutRepairCost))
		{
			if (CurrencyComponent->SpendGold(OutRepairCost))
			{
				OnForgeActionCompleted.Broadcast(InstanceId, ActionType);
				DeactivateWidget();
			}
		}

		return;
	}

	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		const int32 EnhanceCost = InventoryComponent->GetEnhanceCost(InstanceId);
		UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();

		if (!CurrencyComponent || !InventoryComponent->IsEnhanceableItem(InstanceId) || EnhanceCost <= 0 || 
			!CurrencyComponent->HasEnoughGold(EnhanceCost))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ForgeCurrency] Enhance failed. Not enough Gold. Cost=%d"), EnhanceCost);
			RefreshInfo();
			return;
		}

		if (!CurrencyComponent->SpendGold(EnhanceCost))
		{
			RefreshInfo();
			return;
		}

		int32 OutEnhanceCost = 0;
		EMuksiEnhanceResult EnhanceResult = EMuksiEnhanceResult::None;

		if (!InventoryComponent->EnhanceItem(InstanceId, OutEnhanceCost, EnhanceResult))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ForgeCurrency] EnhanceItem failed after gold spent. Cost=%d"), EnhanceCost);
			OnForgeActionCompleted.Broadcast(InstanceId, ActionType);
			RefreshInfo();
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[ForgeCurrency] Enhance attempted. Cost=%d Remain=%d Result=%d"),
			EnhanceCost,
			CurrencyComponent->GetGold(),
			static_cast<int32>(EnhanceResult));

		OnForgeActionCompleted.Broadcast(InstanceId, ActionType);
		RefreshInfo();
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

UPlayerCurrencyComponent* UWidget_ForgeConfirmPopup::GetCurrencyComponent() const
{
	if (UMuksiPlayerDataSubsystem* PlayerDataSubsystem = UMuksiPlayerDataSubsystem::Get(this))
	{
		return PlayerDataSubsystem->GetPlayerCurrencyComponent();
	}

	return nullptr;
}

int32 UWidget_ForgeConfirmPopup::GetCurrentActionCost(const UInventoryComponent* InventoryComponent) const
{
	if (!InventoryComponent)
	{
		return 0;
	}

	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		return InventoryComponent->GetEnhanceCost(InstanceId);
	}

	return InventoryComponent->GetRepairCost(InstanceId);
}

bool UWidget_ForgeConfirmPopup::CanAffordCurrentAction(const UInventoryComponent* InventoryComponent) const
{
	const UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();
	const int32 Cost = GetCurrentActionCost(InventoryComponent);

	return CurrencyComponent && CurrencyComponent->HasEnoughGold(Cost);
}

void UWidget_ForgeConfirmPopup::RefreshGoldInfo(const UInventoryComponent* InventoryComponent)
{
	const UPlayerCurrencyComponent* CurrencyComponent = GetCurrencyComponent();
	const int32 CurrentGold = CurrencyComponent ? CurrencyComponent->GetGold() : 0;
	const int32 Cost = GetCurrentActionCost(InventoryComponent);
	const bool bCanAfford = CurrencyComponent && CurrencyComponent->HasEnoughGold(Cost);

	if (GoldText)
	{
		GoldText->SetText(FText::FromString(
			FString::Printf(TEXT("Gold: %d / Cost: %d"), CurrentGold, Cost)
		));
	}

	if (NotEnoughGoldText)
	{
		NotEnoughGoldText->SetVisibility(bCanAfford ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		NotEnoughGoldText->SetText(FText::FromString(TEXT("Not enough Gold.")));
	}
}