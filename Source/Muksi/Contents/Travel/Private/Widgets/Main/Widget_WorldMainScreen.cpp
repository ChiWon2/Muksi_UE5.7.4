#include "Muksi/Contents/Travel/Public/Widgets/Main/Widget_WorldMainScreen.h"

#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"

void UWidget_WorldMainScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsActivated())
		SetWorldMapLayerActive(true);
}

void UWidget_WorldMainScreen::NativeDestruct()
{
	SetWorldMapLayerActive(false);

	Super::NativeDestruct();
}

void UWidget_WorldMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	SetWorldMapLayerActive(true);
}

void UWidget_WorldMainScreen::NativeOnDeactivated()
{
	SetWorldMapLayerActive(false);

	Super::NativeOnDeactivated();
}

FReply UWidget_WorldMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}

void UWidget_WorldMainScreen::SetWorldMapLayerActive(bool bActive)
{
	if (WorldMapMarkerLayer)
		WorldMapMarkerLayer->SetLayerActive(bActive);
	else if (bActive)
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Add WorldMapMarkerLayer to the world MainScreen WBP."));
}
