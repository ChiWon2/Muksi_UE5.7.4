#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"

#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

void UWorldMapMarkerLayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!IsConfigured())
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Configure MarkerCanvas and MarkerWidgetClass in %s."), *GetClass()->GetName());
}

void UWorldMapMarkerLayerWidget::NativeDestruct()
{
	SetLayerActive(false);
	Super::NativeDestruct();
}

bool UWorldMapMarkerLayerWidget::IsConfigured() const
{
	return MarkerCanvas && MarkerWidgetClass && !MarkerWidgetClass->HasAnyClassFlags(CLASS_Abstract);
}

void UWorldMapMarkerLayerWidget::SetLayerActive(bool bActive)
{
	bLayerActive = bActive;
	SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	if (!bActive)
	{
		if (MarkerController.IsValid())
			MarkerController->DetachMarkerLayer(this);

		MarkerController.Reset();
		return;
	}

	if (IsDesignTime() || MarkerController.IsValid())
		return;

	AWorldUIManager* UIManager = Cast<AWorldUIManager>(UGameplayStatics::GetActorOfClass(this, AWorldUIManager::StaticClass()));
	if (UIManager && UIManager->GetWorldMapMarkerUIComponent())
		UIManager->GetWorldMapMarkerUIComponent()->AttachMarkerLayer(this);
}

void UWorldMapMarkerLayerWidget::SetMarkerController(UWorldMapMarkerUIComponent* Controller)
{
	MarkerController = Controller;
}

bool UWorldMapMarkerLayerWidget::AddMarkerWidget(UWorldMapMarkerWidget* Widget)
{
	if (!MarkerCanvas || !Widget)
		return false;

	UCanvasPanelSlot* CanvasSlot = MarkerCanvas->AddChildToCanvas(Widget);
	if (!CanvasSlot)
		return false;

	CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	CanvasSlot->SetAlignment(FVector2D::ZeroVector);
	CanvasSlot->SetAutoSize(true);
	return true;
}

void UWorldMapMarkerLayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bLayerActive && MarkerCanvas && MarkerController.IsValid())
		MarkerController->UpdateMarkerLayer(this, MarkerCanvas->GetCachedGeometry());
}
