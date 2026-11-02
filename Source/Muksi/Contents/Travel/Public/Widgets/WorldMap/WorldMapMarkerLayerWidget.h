#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"
#include "WorldMapMarkerLayerWidget.generated.h"

class UCanvasPanel;
class UWorldMapMarkerUIComponent;
class UWorldMapMarkerWidget;
class UDestinationMarkerWidget;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UWorldMapMarkerLayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "World Map Markers")
	void SetLayerActive(bool bActive);

	bool IsLayerActive() const { return bLayerActive; }
	bool IsConfigured() const;
	TSubclassOf<UWorldMapMarkerWidget> GetMarkerWidgetClass(EWorldMapMarkerType MarkerType) const;
	void SetMarkerController(UWorldMapMarkerUIComponent* Controller);
	bool AddMarkerWidget(UWorldMapMarkerWidget* Widget);
	bool HasDestinationMarker() const;
	void SetDestinationPosition(const FVector2D& Position);
	void SetDestinationVisible(bool bVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MarkerCanvas;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World Map Markers", meta = (AllowAbstract = "false"))
	TSubclassOf<UWorldMapMarkerWidget> MarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World Map Markers", meta = (AllowAbstract = "false"))
	TSubclassOf<UWorldMapMarkerWidget> PlayerMarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "World Map Markers", meta = (AllowAbstract = "false"))
	TSubclassOf<UDestinationMarkerWidget> DestinationMarkerWidgetClass;

private:
	void CreateDestinationMarker();
	void RemoveDestinationMarker();

	UPROPERTY(Transient)
	TObjectPtr<UDestinationMarkerWidget> DestinationMarkerInstance;

	TWeakObjectPtr<UWorldMapMarkerUIComponent> MarkerController;
	bool bLayerActive = false;
};
