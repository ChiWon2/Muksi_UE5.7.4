#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MuksiPlayerDataSubsystem.generated.h"

class AMuksiWorldCharacter;
class UStatComponent;
class UInventoryComponent;
class UEquipmentComponent;

UCLASS()
class MUKSI_API UMuksiPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Player Data", meta = (WorldContext = "WorldContextObject"))
	static UMuksiPlayerDataSubsystem* Get(const UObject* WorldContextObject);

	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	UFUNCTION(BlueprintPure, Category = "Player Data")
	AMuksiWorldCharacter* GetPlayerPawn() const;

	UFUNCTION(BlueprintPure, Category = "Player Data|Component")
	UStatComponent* GetPlayerStatComponent() const;

	UFUNCTION(BlueprintPure, Category = "Player Data|Component")
	UInventoryComponent* GetPlayerInventoryComponent() const;

	UFUNCTION(BlueprintPure, Category = "Player Data|Component")
	UEquipmentComponent* GetPlayerEquipmentComponent() const;

	UFUNCTION(BlueprintPure, Category = "Player Data|Inventory")
	bool HasItem(FName ItemID, int32 Count = 1) const;

	UFUNCTION(BlueprintPure, Category = "Player Data|Equipment")
	bool IsEquippedItem(FName ItemID) const;
};