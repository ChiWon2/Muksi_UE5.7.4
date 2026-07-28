#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "Widget_ShopInventoryPanel.generated.h"

class UInventoryComponent;
class UPlayerCurrencyComponent;
class UTextBlock;
class UUniformGridPanel;
class UWidget_ItemSlot;

UENUM(BlueprintType)
enum class EShopInventoryPanelMode : uint8
{
	ViewOnly	UMETA(DisplayName = "View Only"),
	Sell		UMETA(DisplayName = "Sell")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopInventoryItemSelected, FGuid, InstanceId);

UCLASS()
class MUKSI_API UWidget_ShopInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shop|Inventory")
	void Refresh();

	UFUNCTION(BlueprintCallable, Category = "Shop|Inventory")
	void SetPanelMode(EShopInventoryPanelMode InMode);

	UFUNCTION(BlueprintCallable, Category = "Shop|Inventory")
	void SetItemTypeFilter(EMuksiItemType InItemTypeFilter);

	UFUNCTION(BlueprintCallable, Category = "Shop|Inventory")
	void SetSelectedInstanceId(FGuid InSelectedInstanceId);

	UPROPERTY(BlueprintAssignable, Category = "Shop|Inventory")
	FOnShopInventoryItemSelected OnItemSelected;

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

	UFUNCTION()
	void HandleItemSlotClicked(FGuid InstanceId);

	void RefreshGold();
	void RefreshInventory();

	bool ShouldShowEntry(const FMuksiInventoryEntry& Entry) const;

	UInventoryComponent* GetInventoryComponent() const;
	UPlayerCurrencyComponent* GetCurrencyComponent() const;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerCurrencyComponent> CachedCurrencyComponent;

	EShopInventoryPanelMode PanelMode = EShopInventoryPanelMode::ViewOnly;
	EMuksiItemType ItemTypeFilter = EMuksiItemType::None;
	FGuid SelectedInstanceId;
};