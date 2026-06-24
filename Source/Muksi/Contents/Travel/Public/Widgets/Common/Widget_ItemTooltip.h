#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "Widget_ItemTooltip.generated.h"

class UTextBlock;

UCLASS()
class MUKSI_API UWidget_ItemTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeTooltip(const FMuksiInventoryEntry& InEntry);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EquipmentStatsText;

private:
	FString BuildEquipmentStatsText(const FMuksiItemStatModifier& Modifier) const;
};