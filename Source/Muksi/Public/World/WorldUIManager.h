#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldUIManager.generated.h"

class UInventoryEquipmentUIComponent;
class UTownDataAsset;
class UTownUIControllerComponent;

UCLASS()
class MUKSI_API AWorldUIManager : public AActor
{
	GENERATED_BODY()

public:
	AWorldUIManager();

	UFUNCTION(BlueprintPure, Category = "World UI")
	UTownUIControllerComponent* GetTownUIControllerComponent() const;

	UFUNCTION(BlueprintPure, Category = "World UI")
	UInventoryEquipmentUIComponent* GetInventoryEquipmentUIComponent() const;

	UFUNCTION(BlueprintCallable, Category = "World UI")
	void OpenTownUI(UTownDataAsset* TownData);

	UFUNCTION(BlueprintCallable, Category = "World UI")
	void CloseTownUI();

	UFUNCTION(BlueprintPure, Category = "World UI")
	bool IsTownUIOpen() const;

	UFUNCTION(BlueprintCallable, Category = "World UI")
	void OpenInventoryEquipmentUI();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTownUIControllerComponent> TownUIControllerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryEquipmentUIComponent> InventoryEquipmentUIController;
};