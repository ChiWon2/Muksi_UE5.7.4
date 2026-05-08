#include "ZoneManager.h"
#include "Controllers/MuksiPlayerController.h"
#include "Kismet/GameplayStatics.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AZoneManager::BeginPlay()
{
	Super::BeginPlay();

	if (AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->SetZoneManager(this);
	}
}

void AZoneManager::RegisterZone(AZoneActor* Zone)
{
	if (!Zone)
	{
		return;
	}

	RegisteredZones.AddUnique(Zone);
}

void AZoneManager::UnregisterZone(AZoneActor* Zone)
{
	if (!Zone)
	{
		return;
	}

	RegisteredZones.Remove(Zone);

	if (CurrentZone.Get() == Zone)
	{
		CurrentZone = nullptr;
	}
}

void AZoneManager::SetCurrentZone(AZoneActor* NewZone)
{
	CurrentZone = NewZone;
}

FZoneData AZoneManager::GetCurrentZoneData() const
{
	if (CurrentZone.IsValid())
	{
		return CurrentZone->GetZoneData();
	}

	return FZoneData();
}