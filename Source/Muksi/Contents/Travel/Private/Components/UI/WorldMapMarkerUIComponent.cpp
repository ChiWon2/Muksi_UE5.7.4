#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"

#include "Muksi/Contents/Travel/Public/Camera/WanderingCameraManager.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

UWorldMapMarkerUIComponent::UWorldMapMarkerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UWorldMapMarkerUIComponent::IsPresentationReady() const
{
	return OwningPlayerController.IsValid() && WanderingCamera.IsValid() && MarkerLayer.IsValid()
		&& MarkerLayer->IsLayerActive() && MarkerLayer->IsConfigured()
		&& MarkerLayer->GetOwningPlayer() == OwningPlayerController.Get();
}

void UWorldMapMarkerUIComponent::StartPresentation(APlayerController* PlayerController, AWanderingCameraManager* CameraManager)
{
	StopPresentation();

	if (!PlayerController || !PlayerController->IsLocalController() || !IsValid(CameraManager))
		return;

	OwningPlayerController = PlayerController;
	WanderingCamera = CameraManager;
	bPresentationStarted = true;

	// Initial discovery covers actors and an active screen created before this manager.
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		TInlineComponentArray<UWorldMapMarkerComponent*> Components;
		It->GetComponents(Components);

		for (UWorldMapMarkerComponent* Component : Components)
			RegisterMarker(Component);
	}

	if (!MarkerLayer.IsValid())
	{
		TArray<UUserWidget*> Layers;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Layers, UWorldMapMarkerLayerWidget::StaticClass(), false);

		for (UUserWidget* Widget : Layers)
		{
			UWorldMapMarkerLayerWidget* Layer = Cast<UWorldMapMarkerLayerWidget>(Widget);
			if (Layer && Layer->IsLayerActive() && Layer->GetOwningPlayer() == PlayerController)
			{
				AttachMarkerLayer(Layer);
				return;
			}
		}
	}

	for (FWorldMapMarkerEntry& Entry : Markers)
		CreateMarkerWidget(Entry);
}

void UWorldMapMarkerUIComponent::ClearMarkerWidgets()
{
	for (FWorldMapMarkerEntry& Entry : Markers)
	{
		SetMarkerVisibility(Entry, false);

		if (Entry.Widget)
			Entry.Widget->RemoveFromParent();

		Entry.Widget = nullptr;
		Entry.bWidgetCreationAttempted = false;
	}
}

void UWorldMapMarkerUIComponent::SetMarkerVisibility(FWorldMapMarkerEntry& Entry, bool bVisible)
{
	if (Entry.Widget)
	{
		const ESlateVisibility Visibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		if (Entry.Widget->GetVisibility() != Visibility)
			Entry.Widget->SetVisibility(Visibility);
	}

	if (Entry.Marker.IsValid())
		Entry.Marker->SetWorldVisualsHidden(bVisible && Entry.Widget && bFarView && bHideVisualsInFarView && Entry.Marker->IconTexture != nullptr);
}

void UWorldMapMarkerUIComponent::StopPresentation()
{
	bPresentationStarted = false;
	ClearMarkerWidgets();
	OwningPlayerController.Reset();
	WanderingCamera.Reset();
	bFarView = false;
}

void UWorldMapMarkerUIComponent::AttachMarkerLayer(UWorldMapMarkerLayerWidget* Layer)
{
	if (!IsValid(Layer) || Layer->GetWorld() != GetWorld() || !Layer->IsLayerActive())
		return;

	if (OwningPlayerController.IsValid() && Layer->GetOwningPlayer() != OwningPlayerController.Get())
		return;

	if (MarkerLayer.Get() != Layer)
	{
		ClearMarkerWidgets();

		if (MarkerLayer.IsValid())
			MarkerLayer->SetMarkerController(nullptr);

		MarkerLayer = Layer;
	}

	Layer->SetMarkerController(this);

	for (FWorldMapMarkerEntry& Entry : Markers)
		CreateMarkerWidget(Entry);
}

void UWorldMapMarkerUIComponent::DetachMarkerLayer(UWorldMapMarkerLayerWidget* Layer)
{
	if (MarkerLayer.Get() != Layer)
		return;

	ClearMarkerWidgets();
	Layer->SetMarkerController(nullptr);
	MarkerLayer.Reset();
}

void UWorldMapMarkerUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopPresentation();

	if (MarkerLayer.IsValid())
		MarkerLayer->SetMarkerController(nullptr);

	MarkerLayer.Reset();

	Super::EndPlay(EndPlayReason);
}

void UWorldMapMarkerUIComponent::RegisterMarker(UWorldMapMarkerComponent* Marker)
{
	if (!IsValid(Marker) || Marker->GetWorld() != GetWorld())
		return;

	Marker->SetMarkerUIController(this);

	if (Markers.ContainsByPredicate([Marker](const FWorldMapMarkerEntry& Entry) { return Entry.Marker.Get() == Marker; }))
		return;

	FWorldMapMarkerEntry& Entry = Markers.AddDefaulted_GetRef();
	Entry.Marker = Marker;
	CreateMarkerWidget(Entry);
}

void UWorldMapMarkerUIComponent::UnregisterMarker(UWorldMapMarkerComponent* Marker)
{
	for (int32 Index = Markers.Num() - 1; Index >= 0; --Index)
	{
		if (Markers[Index].Marker.Get() != Marker)
			continue;

		if (Markers[Index].Widget)
			Markers[Index].Widget->RemoveFromParent();

		Markers.RemoveAtSwap(Index);
	}

	if (IsValid(Marker))
	{
		Marker->SetWorldVisualsHidden(false);
		Marker->SetMarkerUIController(nullptr);
	}
}

void UWorldMapMarkerUIComponent::CreateMarkerWidget(FWorldMapMarkerEntry& Entry)
{
	if (Entry.Widget || Entry.bWidgetCreationAttempted || !Entry.Marker.IsValid() || !IsPresentationReady())
		return;

	Entry.bWidgetCreationAttempted = true;
	Entry.Widget = CreateWidget<UWorldMapMarkerWidget>(OwningPlayerController.Get(), MarkerLayer->GetMarkerWidgetClass());
	if (!Entry.Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Failed to create WBP marker."));
		return;
	}

	if (!MarkerLayer->AddMarkerWidget(Entry.Widget))
	{
		Entry.Widget = nullptr;
		return;
	}

	Entry.Widget->TakeWidget();
	if (!Entry.Widget->HasValidMarkerLayout())
	{
		Entry.Widget->RemoveFromParent();
		Entry.Widget = nullptr;
		return;
	}

	Entry.Widget->UpdateMarkerAppearance(Entry.Marker.Get());
	Entry.Widget->ForceLayoutPrepass();
	Entry.Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void UWorldMapMarkerUIComponent::UpdateMarkerLayer(UWorldMapMarkerLayerWidget* Layer, const FGeometry& CanvasGeometry)
{
	if (MarkerLayer.Get() != Layer)
		return;

	if (!bPresentationStarted)
		return;

	if (!OwningPlayerController.IsValid() || !WanderingCamera.IsValid())
	{
		StopPresentation();
		return;
	}

	if (!IsPresentationReady())
	{
		ClearMarkerWidgets();
		return;
	}

	const FVector2D CanvasSize = CanvasGeometry.GetLocalSize();
	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
	{
		ClearMarkerWidgets();
		return;
	}

	APlayerController* PlayerController = OwningPlayerController.Get();
	const float EnterDistance = FMath::Max(FarViewEnterDistance, 1.0f);
	const float ExitDistance = FMath::Clamp(FarViewExitDistance, 0.0f, EnterDistance - 1.0f);
	const float ZoomDistance = WanderingCamera->GetZoomDistance();
	if (bFarView && ZoomDistance <= ExitDistance)
		bFarView = false;
	else if (!bFarView && ZoomDistance >= EnterDistance)
		bFarView = true;

	const bool bLayerEnabled = bFarView || bShowMarkersInNearView;
	for (int32 Index = Markers.Num() - 1; Index >= 0; --Index)
	{
		FWorldMapMarkerEntry& Entry = Markers[Index];
		UWorldMapMarkerComponent* Marker = Entry.Marker.Get();
		if (!Marker)
		{
			if (Entry.Widget)
				Entry.Widget->RemoveFromParent();

			Markers.RemoveAtSwap(Index);
			continue;
		}

		CreateMarkerWidget(Entry);
		if (!Entry.Widget)
		{
			SetMarkerVisibility(Entry, false);
			continue;
		}

		const AActor* MarkerOwner = Marker->GetOwner();
		const bool bEnabled = bLayerEnabled && Marker->bMarkerEnabled && IsValid(MarkerOwner) && !MarkerOwner->IsHidden();
		FVector2D ScreenPosition = FVector2D::ZeroVector;
		FVector2D Position = FVector2D::ZeroVector;
		const bool bProjected = bEnabled && PlayerController->ProjectWorldLocationToScreen(Marker->GetComponentLocation(), ScreenPosition, false);
		if (bProjected)
			USlateBlueprintLibrary::ScreenToWidgetLocal(PlayerController, CanvasGeometry, ScreenPosition, Position, false);

		const bool bOnScreen = bProjected && Position.X >= 0.0f && Position.Y >= 0.0f && Position.X <= CanvasSize.X && Position.Y <= CanvasSize.Y;
		if (!bOnScreen)
		{
			SetMarkerVisibility(Entry, false);
			continue;
		}

		// A collapsed widget must participate in the prepass before measuring it.
		if (Entry.Widget->GetVisibility() != ESlateVisibility::HitTestInvisible)
			Entry.Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
		Entry.Widget->UpdateMarkerAppearance(Marker);
		FVector2D DrawSize = FVector2D::ZeroVector;
		FVector2D IconCenter = FVector2D::ZeroVector;
		if (!Entry.Widget->GetMarkerLayout(false, DrawSize, IconCenter))
		{
			SetMarkerVisibility(Entry, false);
			continue;
		}

		bool bNameOnLeft = false;
		if (Position.X + DrawSize.X - IconCenter.X > CanvasSize.X)
		{
			FVector2D LeftSize = FVector2D::ZeroVector;
			FVector2D LeftIconCenter = FVector2D::ZeroVector;
			if (Entry.Widget->GetMarkerLayout(true, LeftSize, LeftIconCenter) && Position.X >= LeftIconCenter.X)
			{
				bNameOnLeft = true;
				IconCenter = LeftIconCenter;
			}
		}

		Entry.Widget->SetNameOnLeft(bNameOnLeft);
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Entry.Widget->Slot);
		if (!CanvasSlot)
		{
			SetMarkerVisibility(Entry, false);
			continue;
		}

		CanvasSlot->SetPosition(Position - IconCenter);
		CanvasSlot->SetZOrder(Marker->SortPriority);
		SetMarkerVisibility(Entry, true);
	}
}
