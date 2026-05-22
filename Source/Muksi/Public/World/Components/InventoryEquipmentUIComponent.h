#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryEquipmentUIComponent.generated.h"

class UWidget_ActivatableBase;

UCLASS(ClassGroup = (World), meta = (BlueprintSpawnableComponent))
class MUKSI_API UInventoryEquipmentUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryEquipmentUIComponent();

	void OpenInventoryEquipmentUI();
	void CloseInventoryEquipmentUI();

	bool IsInventoryEquipmentUIOpen() const { return CurrentInventoryEquipmentWidget != nullptr; }

protected:
	UPROPERTY(EditAnywhere, Category = "World UI")
	TSoftClassPtr<UWidget_ActivatableBase> InventoryEquipmentWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_ActivatableBase> CurrentInventoryEquipmentWidget = nullptr;
};