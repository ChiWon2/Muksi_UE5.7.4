#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Travel/Public/Data/Shops/ShopDataAsset.h"
#include "Widget_ShopItemEntry.generated.h"

class UMuksiItemDataAsset;

class UButton;
class UTextBlock;
class UImage;
class UWidget;

DECLARE_DELEGATE_OneParam(FOnShopItemEntryClicked, const FShopItemEntry&);

UCLASS()
class MUKSI_API UWidget_ShopItemEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FShopItemEntry& InEntry, const UMuksiItemDataAsset* InItemData);
	void SetSelected(bool bInSelected);

	FOnShopItemEntryClicked OnClicked;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly)
	FShopItemEntry Entry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Root;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Price;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Condition;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> SelectedBorder;

	UFUNCTION()
	void HandleClicked();
};