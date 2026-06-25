#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MuksiItemDataSubsystem.generated.h"

class UMuksiItemDatabase;
class UMuksiItemDataAsset;

UCLASS()
class MUKSI_API UMuksiItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Item Data", meta = (WorldContext = "WorldContextObject"))
	static UMuksiItemDataSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "Item Data")
	UMuksiItemDataAsset* FindItemData(FName ItemID) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiItemDatabase> ItemDatabase = nullptr;
};