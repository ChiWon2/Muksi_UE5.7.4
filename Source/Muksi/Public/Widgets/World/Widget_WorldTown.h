#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/World/Data/TownInteractionData.h"
#include "Widget_WorldTown.generated.h"

class UButton;
class UImage;
class UPanelWidget;
class UCommonTextBlock;
class UTownDataAsset;
class UWidget_ActivatableBase;
class UWidget_TownInteractionButton;

UCLASS()
class MUKSI_API UWidget_WorldTown : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Town")
	void InitializeTown(UTownDataAsset* InTownData);

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> TownNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ThumbnailImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> InteractionListPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Town")
	TSubclassOf<UWidget_TownInteractionButton> InteractionButtonWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Town|Forge")
	TSoftClassPtr<UWidget_ActivatableBase> ForgeWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Town|Shrine")
	TSoftClassPtr<UWidget_ActivatableBase> ShrineWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Town|TrainingGrounds")
	TSoftClassPtr<UWidget_ActivatableBase> TrainingGroundsWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Town|TangClan")
	TSoftClassPtr<UWidget_ActivatableBase> TangClanWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Town|Shop")
	TSoftClassPtr<UWidget_ActivatableBase> ShopWidgetClass;


	UFUNCTION()
	void HandleCloseButtonClicked();

private:
	UPROPERTY()
	TObjectPtr<UTownDataAsset> CurrentTownData;

	void RefreshTown();
	void RebuildInteractionButtons();
	void HandleInteractionClicked(const FTownInteractionData& InteractionData);

	void OpenTrainingGrounds(const FTownInteractionData& InteractionData);
	void OpenForge(const FTownInteractionData& InteractionData);
	void OpenShrine(const FTownInteractionData& InteractionData);
	void OpenTangClan(const FTownInteractionData& InteractionData);
	void OpenShop(const FTownInteractionData& InteractionData);
	void OpenCustomInteraction(const FTownInteractionData& InteractionData);
};