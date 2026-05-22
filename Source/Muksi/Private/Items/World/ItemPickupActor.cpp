#include "Items/World/ItemPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"

AItemPickupActor::AItemPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	bOneTimeUse = true;
	bUsed = false;
	InteractionName = FText::FromString(TEXT("Pick Up"));

	if (InteractionSphere)
	{
		InteractionSphere->SetSphereRadius(150.f);
	}

	if (Mesh)
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItemPickupActor::Interact(AActor* Interactor)
{
	if (bOneTimeUse && bUsed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Already used. Actor=%s"), *GetNameSafe(this));
		return;
	}

	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Failed: Interactor is null. Actor=%s"), *GetNameSafe(this));
		return;
	}

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Failed: ItemData is null. Actor=%s"), *GetNameSafe(this));
		return;
	}

	if (Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Failed: Quantity is invalid. Actor=%s Quantity=%d"), *GetNameSafe(this), Quantity);
		return;
	}

	UInventoryComponent* InventoryComponent = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Failed: InventoryComponent is null. Interactor=%s"), *GetNameSafe(Interactor));
		return;
	}

	const bool bAdded = InventoryComponent->AddItem(ItemData, Quantity);
	if (!bAdded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] AddItem failed. Item=%s Quantity=%d Interactor=%s"),
			*GetNameSafe(ItemData),
			Quantity,
			*GetNameSafe(Interactor));
		return;
	}

	bUsed = true;

	UE_LOG(LogTemp, Log, TEXT("[ItemPickup] Picked up item. Item=%s DisplayName=%s Quantity=%d Interactor=%s"),
		*GetNameSafe(ItemData),
		*ItemData->DisplayName.ToString(),
		Quantity,
		*GetNameSafe(Interactor));

	if (bDestroyOnPickupSuccess)
	{
		Destroy();
	}
}