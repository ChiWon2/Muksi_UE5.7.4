#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ShopInventoryPanel.generated.h"

class UInventoryComponent;
class UPlayerCurrencyComponent;
class UTextBlock;
class UUniformGridPanel;
class UWidget_ItemSlot;

UCLASS()
class MUKSI_API UWidget_ShopInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shop|Inventory")
	void Refresh();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGridPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Inventory")
	TSubclassOf<UWidget_ItemSlot> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Inventory",
		meta = (ClampMin = "1"))
	int32 InventoryColumnCount = 5;

private:
	UFUNCTION()
	void HandleGoldChanged(int32 NewGold);

	void RefreshGold();
	void RefreshInventory();

	UInventoryComponent* GetInventoryComponent() const;
	UPlayerCurrencyComponent* GetCurrencyComponent() const;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerCurrencyComponent> CachedCurrencyComponent;
};