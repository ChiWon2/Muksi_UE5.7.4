#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_WorldMainScreen.generated.h"

class UWorldMapMarkerLayerWidget;

UCLASS()
class MUKSI_API UWidget_WorldMainScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWorldMapMarkerLayerWidget> WorldMapMarkerLayer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DefaultFocusWidget;

private:
	void SetWorldMapLayerActive(bool bActive);
};
