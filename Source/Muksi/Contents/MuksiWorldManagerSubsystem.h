#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/WorldSubsystem.h"
#include "MuksiWorldManagerSubsystem.generated.h"

/**
 * 현재 World에 존재하는 Manager Actor를 타입별로 등록하고 제공한다.
 */
UCLASS()
class MUKSI_API UMuksiWorldManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TWeakObjectPtr<AActor>> RegisteredManagers;
public:
	static UMuksiWorldManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Muksi|World Manager")
	bool RegisterManagerByClass(TSubclassOf<AActor> ManagerClass, AActor* Manager);

	UFUNCTION(BlueprintCallable, Category = "Muksi|World Manager")
	bool UnregisterManagerByClass(TSubclassOf<AActor> ManagerClass, AActor* Manager);

	UFUNCTION(BlueprintPure, Category = "Muksi|World Manager")
	AActor* GetManagerByClass(TSubclassOf<AActor> ManagerClass) const;

	UFUNCTION(BlueprintPure, Category = "Muksi|World Manager")
	bool HasManagerByClass(TSubclassOf<AActor> ManagerClass) const;

public:
	template<typename T>
	bool RegisterManager(T* Manager);

	template<typename T>
	bool UnregisterManager(T* Manager);

	template<typename T>
	T* GetManager() const;

	template<typename T>
	bool HasManager() const;

};

template<typename T>
bool UMuksiWorldManagerSubsystem::RegisterManager(T* Manager)
{
	static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "T must inherit from AActor.");

	return RegisterManagerByClass(T::StaticClass(), Manager);
}

template<typename T>
bool UMuksiWorldManagerSubsystem::UnregisterManager(T* Manager)
{
	static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "T must inherit from AActor.");

	return UnregisterManagerByClass(T::StaticClass(), Manager);
}

template<typename T>
T* UMuksiWorldManagerSubsystem::GetManager() const
{
	static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "T must inherit from AActor.");

	return Cast<T>(GetManagerByClass(T::StaticClass()));
}

template<typename T>
bool UMuksiWorldManagerSubsystem::HasManager() const
{
	static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "T must inherit from AActor.");

	return HasManagerByClass(T::StaticClass());
}