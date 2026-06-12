#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Data/ShopDataAsset.h"
#include "Widgets/Widget_ActivatableBase.h"

#include "Widget_Shop.generated.h"

class UButton;
class UTextBlock;
class UUniformGridPanel;
class UInventoryComponent;
class UMuksiItemDataAsset;
class UWidget_ShopItemEntry;

UENUM(BlueprintType)
enum class EShopTab : uint8
{
	Buy		UMETA(DisplayName = "Buy"),
	Sell	UMETA(DisplayName = "Sell"),
	ReBuy	UMETA(DisplayName = "ReBuy")
};

UCLASS()
class MUKSI_API UWidget_Shop : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetShopData(UShopDataAsset* InShopData);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TObjectPtr<UShopDataAsset> ShopData;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	FShopItemEntry SelectedShopItem;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	bool bHasSelectedItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Test")
	int32 CurrentGold = 10000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	TSubclassOf<UWidget_ShopItemEntry> ShopItemEntryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 ShopItemColumnCount = 2;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	EShopTab CurrentTab = EShopTab::Buy;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> ItemGridPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ShopTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SelectedItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SelectedItemDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Price;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CurrentGold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AfterGold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Buy;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabBuy;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabSell;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabRebuy;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_EmptyMessage;

	UFUNCTION()
	void HandleBuyClicked();

	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandleBuyTabClicked();

	UFUNCTION()
	void HandleSellTabClicked();

	UFUNCTION()
	void HandleRebuyTabClicked();

	void RefreshShop();
	void RefreshSelection();
	void SelectShopItem(const FShopItemEntry& Entry);

	void SetCurrentTab(EShopTab NewTab);
	void RefreshTabContent();
	void ClearItemGrid();
	void ShowEmptyMessage(const FText& Message);

	bool CanBuySelectedItem() const;
	bool TryBuySelectedItem();

	UInventoryComponent* GetInventoryComponent() const;
};