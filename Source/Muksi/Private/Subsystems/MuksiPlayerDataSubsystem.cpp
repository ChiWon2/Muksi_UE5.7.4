#include "Subsystems/MuksiPlayerDataSubsystem.h"

#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/EquipmentComponent.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UMuksiPlayerDataSubsystem* UMuksiPlayerDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !GEngine)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	return GameInstance ? GameInstance->GetSubsystem<UMuksiPlayerDataSubsystem>() : nullptr;
}

bool UMuksiPlayerDataSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);

		return FoundClasses.IsEmpty();
	}

	return false;
}

AMuksiWorldCharacter* UMuksiPlayerDataSubsystem::GetPlayerPawn() const
{
	return GetPlayerWorldCharacter();
}

AMuksiWorldCharacter* UMuksiPlayerDataSubsystem::GetPlayerWorldCharacter() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return nullptr;
	}

	return Cast<AMuksiWorldCharacter>(PlayerController->GetPawn());
}

UStatComponent* UMuksiPlayerDataSubsystem::GetPlayerStatComponent() const
{
	const AMuksiWorldCharacter* PlayerPawn = GetPlayerPawn();

	UE_LOG(LogTemp, Warning,
		TEXT("[PlayerDataSubsystem] Pawn=%s Class=%s"),
		*GetNameSafe(PlayerPawn),
		PlayerPawn ? *PlayerPawn->GetClass()->GetName() : TEXT("None"));

	return PlayerPawn ? PlayerPawn->GetStatComponent() : nullptr;
}	

UInventoryComponent* UMuksiPlayerDataSubsystem::GetPlayerInventoryComponent() const
{
	const AMuksiWorldCharacter* PlayerPawn = GetPlayerPawn();
	return PlayerPawn ? PlayerPawn->GetInventoryComponent() : nullptr;
}

UEquipmentComponent* UMuksiPlayerDataSubsystem::GetPlayerEquipmentComponent() const
{
	const AMuksiWorldCharacter* PlayerPawn = GetPlayerPawn();
	return PlayerPawn ? PlayerPawn->GetEquipmentComponent() : nullptr;
}

UPlayerCurrencyComponent* UMuksiPlayerDataSubsystem::GetPlayerCurrencyComponent() const
{
	const AMuksiWorldCharacter* PlayerPawn = GetPlayerPawn();
	return PlayerPawn ? PlayerPawn->GetPlayerCurrencyComponent() : nullptr;
}

void UMuksiPlayerDataSubsystem::LogPlayerDataComponents() const
{
	const AMuksiWorldCharacter* PlayerCharacter = GetPlayerWorldCharacter();
	const UStatComponent* StatComponent = GetPlayerStatComponent();
	const UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();
	const UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	const UPlayerCurrencyComponent* CurrencyComponent = GetPlayerCurrencyComponent();

	UE_LOG(LogTemp, Log,
		TEXT("[PlayerDataTest] Pawn=%s Stat=%s Inventory=%s Equipment=%s Currency=%s Gold=%d"),
		*GetNameSafe(PlayerCharacter),
		*GetNameSafe(StatComponent),
		*GetNameSafe(InventoryComponent),
		*GetNameSafe(EquipmentComponent),
		*GetNameSafe(CurrencyComponent),
		CurrencyComponent ? CurrencyComponent->GetGold() : -1);
}

bool UMuksiPlayerDataSubsystem::HasItem(FName ItemID, int32 Count) const
{
	const UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();
	return InventoryComponent ? InventoryComponent->HasItem(ItemID, Count) : false;
}

bool UMuksiPlayerDataSubsystem::IsEquippedItem(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return false;
	}

	const UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent)
	{
		return false;
	}

	for (const TPair<EMuksiEquipmentSlot, FGuid>& Pair : EquipmentComponent->GetEquippedItemIds())
	{
		FMuksiInventoryEntry EquippedEntry;
		if (!EquipmentComponent->GetEquippedItem(Pair.Key, EquippedEntry))
		{
			continue;
		}

		if (EquippedEntry.ItemData && EquippedEntry.ItemData->ItemID == ItemID)
		{
			return true;
		}
	}

	return false;
}