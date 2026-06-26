#include "Subsystems/MuksiItemDataSubsystem.h"

#include "MuksiSettings/MuksiSubsystemSettings.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDatabase.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UMuksiItemDataSubsystem* UMuksiItemDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !GEngine)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;

	return GameInstance ? GameInstance->GetSubsystem<UMuksiItemDataSubsystem>() : nullptr;
}

void UMuksiItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMuksiSubsystemSettings* Settings = GetDefault<UMuksiSubsystemSettings>();
	if (Settings && !Settings->ItemDatabase.IsNull())
	{
		ItemDatabase = Settings->ItemDatabase.LoadSynchronous();
	}

	if (!ItemDatabase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDataSubsystem] ItemDatabase is not configured."));
	}
}

UMuksiItemDataAsset* UMuksiItemDataSubsystem::FindItemData(FName ItemID) const
{
	if (!ItemDatabase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDataSubsystem] ItemDatabase is null."));
		return nullptr;
	}

	return ItemDatabase ? ItemDatabase->FindItemData(ItemID) : nullptr;
}
