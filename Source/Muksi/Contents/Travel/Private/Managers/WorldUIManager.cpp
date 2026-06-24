#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"

#include "Muksi/Contents/Travel/Public/Components/UI/InventoryEquipmentUIComponent.h"
#include "Muksi/Contents/Travel/Public/Components/UI/TownUIControllerComponent.h"

AWorldUIManager::AWorldUIManager()
{
	PrimaryActorTick.bCanEverTick = false;

	TownUIControllerComponent =
		CreateDefaultSubobject<UTownUIControllerComponent>(TEXT("TownUIControllerComponent"));

	InventoryEquipmentUIController =
		CreateDefaultSubobject<UInventoryEquipmentUIComponent>(TEXT("InventoryEquipmentUIController"));
}

UTownUIControllerComponent* AWorldUIManager::GetTownUIControllerComponent() const
{
	return TownUIControllerComponent.Get();
}

UInventoryEquipmentUIComponent* AWorldUIManager::GetInventoryEquipmentUIComponent() const
{
	return InventoryEquipmentUIController.Get();
}

void AWorldUIManager::OpenTownUI(UTownDataAsset* TownData)
{
	if (!TownUIControllerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: TownUIControllerComponent is null"));
		return;
	}

	TownUIControllerComponent->OpenTownUI(TownData);
}

void AWorldUIManager::CloseTownUI()
{
	if (!TownUIControllerComponent)
	{
		return;
	}

	TownUIControllerComponent->CloseTownUI();
}

bool AWorldUIManager::IsTownUIOpen() const
{
	return TownUIControllerComponent && TownUIControllerComponent->IsTownUIOpen();
}

void AWorldUIManager::OpenInventoryEquipmentUI()
{
	if (!InventoryEquipmentUIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenInventoryEquipmentUI failed: InventoryEquipmentUIController is null"));
		return;
	}

	InventoryEquipmentUIController->OpenInventoryEquipmentUI();
}