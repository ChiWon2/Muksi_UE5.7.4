#include "Controllers/PlayerMode/PlayerMode_World.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/Travel/Public/Interaction/TownInteractionPoint.h"
#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Contents/Travel/Public/Zones/ZoneActor.h"
#include "Muksi/Contents/Travel/Public/Data/Towns/TownDataAsset.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Controller.h"

void UPlayerMode_World::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	if (!PC)
	{
		return;
	}

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;

	FollowTime = 0.0f;
	CachedDestination = FVector::ZeroVector;
}

void UPlayerMode_World::ExitMode()
{
	// TEST

	Super::ExitMode();

	FollowTime = 0.0f;
	CachedDestination = FVector::ZeroVector;
}

void UPlayerMode_World::HandleWorldInputStarted(AMuksiWorldCharacter* WorldCharacter)
{
	FollowTime = 0.0f;

	if (PC)
	{
		PC->StopMovement();
	}
}

void UPlayerMode_World::HandleSetDestinationTriggered(AMuksiWorldCharacter* WorldCharacter)
{
	if (!PC || !WorldCharacter || !WorldCharacter->GetWorld())
	{
		return;
	}

	FollowTime += WorldCharacter->GetWorld()->GetDeltaSeconds();

	FVector HitLocation;
	if (TryGetCursorHitLocation(HitLocation))
	{
		CachedDestination = HitLocation;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, CachedDestination);
	}
}

void UPlayerMode_World::HandleSetDestinationReleased(AMuksiWorldCharacter* WorldCharacter)
{
	if (!PC)
	{
		FollowTime = 0.0f;
		return;
	}

	if (FollowTime <= ShortPressThreshold)
	{
		FVector HitLocation;
		if (TryGetCursorHitLocation(HitLocation))
		{
			CachedDestination = HitLocation;
		}

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, CachedDestination);
	}

	FollowTime = 0.0f;
}

bool UPlayerMode_World::TryGetCursorHitLocation(FVector& OutLocation) const
{
	if (!PC)
	{
		return false;
	}

	FHitResult Hit;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		OutLocation = Hit.Location;
		return true;
	}

	return false;
}

void UPlayerMode_World::HandleInteract(AMuksiWorldCharacter* WorldCharacter)
{
	if (!WorldCharacter)
	{
		return;
	}

	ATownInteractionPoint* InteractionTarget = WorldCharacter->GetCurrentInteractionTarget();
	if (!InteractionTarget)
	{
		return;
	}

	HandleTownInteraction(InteractionTarget, WorldCharacter);
}

void UPlayerMode_World::HandleTownInteraction(ATownInteractionPoint* InteractionPoint, AMuksiWorldCharacter* WorldCharacter)
{
	if (!InteractionPoint)
	{
		return;
	}

	if (UTownDataAsset* TownData = InteractionPoint->GetTownDataAsset())
	{
		InteractionPoint->Interact(WorldCharacter);
		OpenTownUIFromWorld(TownData);
		return;
	}

	InteractionPoint->Interact(WorldCharacter);
}

void UPlayerMode_World::HandleOpenInventoryEquipment(AMuksiWorldCharacter* WorldCharacter)
{
	OpenInventoryEquipmentFromWorld();
}

void UPlayerMode_World::OpenTownUIFromWorld(UTownDataAsset* TownData)
{
	if (!TownData)
	{
		return;
	}

	AWorldUIManager* WorldUIManager = FindWorldUIManager();
	if (!WorldUIManager)
	{
		return;
	}

	WorldUIManager->OpenTownUI(TownData);
}

void UPlayerMode_World::CloseTownUIFromWorld()
{
	AWorldUIManager* WorldUIManager = FindWorldUIManager();
	if (!WorldUIManager)
	{
		return;
	}

	WorldUIManager->CloseTownUI();
}

bool UPlayerMode_World::IsTownUIOpen() const
{
	const AWorldUIManager* WorldUIManager = FindWorldUIManager();
	return WorldUIManager && WorldUIManager->IsTownUIOpen();
}

void UPlayerMode_World::OpenInventoryEquipmentFromWorld()
{
	AWorldUIManager* WorldUIManager = FindWorldUIManager();
	if (!WorldUIManager)
	{
		return;
	}

	WorldUIManager->OpenInventoryEquipmentUI();
}

AWorldUIManager* UPlayerMode_World::FindWorldUIManager() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	AWorldUIManager* WorldUIManager = Cast<AWorldUIManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AWorldUIManager::StaticClass())
	);

	if (!WorldUIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldUIManager not found in current level"));
	}

	return WorldUIManager;
}