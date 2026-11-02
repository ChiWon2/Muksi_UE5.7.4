#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldMapMarkerLayerWidget.generated.h"

class UCanvasPanel;
class UWorldMapMarkerUIComponent;
class UWorldMapMarkerWidget;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UWorldMapMarkerLayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "World Map Markers")
	void SetLayerActive(bool bActive);

	bool IsLayerActive() const { return bLayerActive; }
	bool IsConfigured() const;
	TSubclassOf<UWorldMapMarkerWidget> GetMarkerWidgetClass() const { return MarkerWidgetClass; }
	void SetMarkerController(UWorldMapMarkerUIComponent* Controller);
	bool AddMarkerWidget(UWorldMapMarkerWidget* Widget);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MarkerCanvas;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World Map Markers", meta = (AllowAbstract = "false"))
	TSubclassOf<UWorldMapMarkerWidget> MarkerWidgetClass;

private:
	TWeakObjectPtr<UWorldMapMarkerUIComponent> MarkerController;
	bool bLayerActive = false;
};
