#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

#include "Engine/World.h"

UMuksiWorldManagerSubsystem* UMuksiWorldManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();

	if (!World)
	{
		return nullptr;
	}

	return World->GetSubsystem<UMuksiWorldManagerSubsystem>();
}

void UMuksiWorldManagerSubsystem::Deinitialize()
{
	RegisteredManagers.Empty();

	Super::Deinitialize();
}

bool UMuksiWorldManagerSubsystem::RegisterManagerByClass(TSubclassOf<AActor> ManagerClass, AActor* Manager)
{
	UClass* ManagerClassPtr = ManagerClass.Get();

	if (!ManagerClassPtr || !IsValid(Manager))
	{
		return false;
	}

	if (Manager->GetWorld() != GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiWorldManagerSubsystem] Manager belongs to another World: %s"), *GetNameSafe(Manager));
		return false;
	}

	if (!Manager->IsA(ManagerClassPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiWorldManagerSubsystem] Manager %s is not derived from %s"), *GetNameSafe(Manager), *GetNameSafe(ManagerClassPtr));
		return false;
	}

	if (const TWeakObjectPtr<AActor>* FoundManager = RegisteredManagers.Find(ManagerClassPtr))
	{
		if (FoundManager->Get() == Manager)
		{
			return true;
		}

		if (FoundManager->IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[MuksiWorldManagerSubsystem] Manager already registered. Class: %s, Existing: %s, New: %s"), *GetNameSafe(ManagerClassPtr), *GetNameSafe(FoundManager->Get()), *GetNameSafe(Manager));
			return false;
		}
	}

	RegisteredManagers.Add(ManagerClassPtr, Manager);

	return true;
}

bool UMuksiWorldManagerSubsystem::UnregisterManagerByClass(TSubclassOf<AActor> ManagerClass, AActor* Manager)
{
	UClass* ManagerClassPtr = ManagerClass.Get();

	if (!ManagerClassPtr || !Manager)
	{
		return false;
	}

	const TWeakObjectPtr<AActor>* FoundManager = RegisteredManagers.Find(ManagerClassPtr);

	if (!FoundManager || FoundManager->Get() != Manager)
	{
		return false;
	}

	RegisteredManagers.Remove(ManagerClassPtr);

	return true;
}

AActor* UMuksiWorldManagerSubsystem::GetManagerByClass(TSubclassOf<AActor> ManagerClass) const
{
	UClass* ManagerClassPtr = ManagerClass.Get();

	if (!ManagerClassPtr)
	{
		return nullptr;
	}

	const TWeakObjectPtr<AActor>* FoundManager = RegisteredManagers.Find(ManagerClassPtr);

	if (!FoundManager)
	{
		return nullptr;
	}

	return FoundManager->Get();
}

bool UMuksiWorldManagerSubsystem::HasManagerByClass(TSubclassOf<AActor> ManagerClass) const
{
	return IsValid(GetManagerByClass(ManagerClass));
}