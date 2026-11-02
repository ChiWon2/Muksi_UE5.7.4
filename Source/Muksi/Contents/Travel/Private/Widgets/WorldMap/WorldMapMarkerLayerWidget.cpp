#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"

#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerWidget.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/DestinationMarkerWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UWorldMapMarkerLayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!IsConfigured())
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Configure MarkerCanvas and MarkerWidgetClass in %s."), *GetClass()->GetName());

	CreateDestinationMarker();
}

void UWorldMapMarkerLayerWidget::NativeDestruct()
{
	SetLayerActive(false);
	RemoveDestinationMarker();
	Super::NativeDestruct();
}

void UWorldMapMarkerLayerWidget::CreateDestinationMarker()
{
	RemoveDestinationMarker();
	if (IsDesignTime())
		return;

	if (!MarkerCanvas || !DestinationMarkerWidgetClass || DestinationMarkerWidgetClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Configure MarkerCanvas and DestinationMarkerWidgetClass in %s."), *GetClass()->GetName());
		return;
	}

	APlayerController* MarkerOwningPlayer = GetOwningPlayer();
	if (!MarkerOwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Cannot create a destination marker without an owning player in %s."), *GetClass()->GetName());
		return;
	}

	DestinationMarkerInstance = CreateWidget<UDestinationMarkerWidget>(MarkerOwningPlayer, DestinationMarkerWidgetClass);
	if (!DestinationMarkerInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Failed to create the destination WBP in %s."), *GetClass()->GetName());
		return;
	}

	UCanvasPanelSlot* DestinationSlot = MarkerCanvas->AddChildToCanvas(DestinationMarkerInstance);
	if (!DestinationSlot)
	{
		RemoveDestinationMarker();
		return;
	}

	DestinationMarkerInstance->TakeWidget();
	if (!DestinationMarkerInstance->IsConfigured())
	{
		RemoveDestinationMarker();
		return;
	}

	DestinationSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	DestinationSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	DestinationSlot->SetAutoSize(true);
	DestinationSlot->SetZOrder(DestinationMarkerInstance->GetMarkerZOrder());
	DestinationMarkerInstance->SetMarkerVisible(false);
}

void UWorldMapMarkerLayerWidget::RemoveDestinationMarker()
{
	if (DestinationMarkerInstance)
	{
		DestinationMarkerInstance->SetMarkerVisible(false);
		DestinationMarkerInstance->RemoveFromParent();
	}

	DestinationMarkerInstance = nullptr;
}

bool UWorldMapMarkerLayerWidget::IsConfigured() const
{
	return MarkerCanvas && MarkerWidgetClass && !MarkerWidgetClass->HasAnyClassFlags(CLASS_Abstract);
}

TSubclassOf<UWorldMapMarkerWidget> UWorldMapMarkerLayerWidget::GetMarkerWidgetClass(EWorldMapMarkerType MarkerType) const
{
	return MarkerType == EWorldMapMarkerType::Player ? PlayerMarkerWidgetClass : MarkerWidgetClass;
}

void UWorldMapMarkerLayerWidget::SetLayerActive(bool bActive)
{
	bLayerActive = bActive;
	SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	if (!bActive)
	{
		SetDestinationVisible(false);
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

bool UWorldMapMarkerLayerWidget::HasDestinationMarker() const
{
	return MarkerCanvas && DestinationMarkerInstance && DestinationMarkerInstance->GetParent() == MarkerCanvas
		&& Cast<UCanvasPanelSlot>(DestinationMarkerInstance->Slot) && DestinationMarkerInstance->IsConfigured();
}

void UWorldMapMarkerLayerWidget::SetDestinationPosition(const FVector2D& Position)
{
	if (!HasDestinationMarker())
		return;

	UCanvasPanelSlot* DestinationSlot = CastChecked<UCanvasPanelSlot>(DestinationMarkerInstance->Slot);
	DestinationSlot->SetPosition(Position);
	DestinationSlot->SetZOrder(DestinationMarkerInstance->GetMarkerZOrder());
}

void UWorldMapMarkerLayerWidget::SetDestinationVisible(bool bVisible)
{
	if (DestinationMarkerInstance)
		DestinationMarkerInstance->SetMarkerVisible(bVisible && bLayerActive);
}

void UWorldMapMarkerLayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bLayerActive && MarkerCanvas && MarkerController.IsValid())
		MarkerController->UpdateMarkerLayer(this, MarkerCanvas->GetCachedGeometry());
}
