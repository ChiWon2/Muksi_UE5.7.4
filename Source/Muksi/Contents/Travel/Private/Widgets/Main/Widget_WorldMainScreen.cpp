#include "Muksi/Contents/Travel/Public/Widgets/Main/Widget_WorldMainScreen.h"

#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"

void UWidget_WorldMainScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsActivated() && WorldMapMarkerLayer)
		WorldMapMarkerLayer->SetLayerActive(true);
}

void UWidget_WorldMainScreen::NativeDestruct()
{
	if (WorldMapMarkerLayer)
		WorldMapMarkerLayer->SetLayerActive(false);

	Super::NativeDestruct();
}

void UWidget_WorldMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (WorldMapMarkerLayer)
		WorldMapMarkerLayer->SetLayerActive(true);
	else
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Add a WorldMapMarkerLayer widget to the world MainScreen WBP."));
}

void UWidget_WorldMainScreen::NativeOnDeactivated()
{
	if (WorldMapMarkerLayer)
		WorldMapMarkerLayer->SetLayerActive(false);

	Super::NativeOnDeactivated();
}

FReply UWidget_WorldMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}
