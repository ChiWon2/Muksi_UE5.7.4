#pragma once

#include "CoreMinimal.h"
#include "Widgets/Components/MuksiCommonButtonBase.h"
#include "Muksi/Contents/World/Data/TownInteractionData.h"
#include "Widget_TownInteractionButton.generated.h"

class UImage;
class UCommonTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FTownInteractionButtonClicked, const FTownInteractionData&);

UCLASS(Abstract, BlueprintType)
class MUKSI_API UWidget_TownInteractionButton : public UMuksiCommonButtonBase
{
	GENERATED_BODY()

public:
	void InitializeInteraction(const FTownInteractionData& InInteractionData);

	FTownInteractionButtonClicked OnTownInteractionClicked;

protected:
	virtual void NativeOnClicked() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> DisplayNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> DescriptionText;

private:
	UPROPERTY()
	FTownInteractionData InteractionData;
};
