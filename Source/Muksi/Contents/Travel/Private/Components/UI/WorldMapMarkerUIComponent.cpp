#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"

#include "Muksi/Contents/Travel/Public/Camera/WanderingCameraManager.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerLayerWidget.h"
#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Math/NumericLimits.h"

UWorldMapMarkerUIComponent::UWorldMapMarkerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UWorldMapMarkerUIComponent::IsPresentationReady(UWorldMapMarkerLayerWidget* Layer) const
{
	return bPresentationStarted && OwningPlayerController.IsValid() && WanderingCamera.IsValid() && IsValid(Layer)
		&& Layer->IsLayerActive() && Layer->IsConfigured()
		&& Layer->GetOwningPlayer() == OwningPlayerController.Get();
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

void UWorldMapMarkerUIComponent::ClearMarkerWidget(FWorldMapMarkerEntry& Entry)
{
	SetMarkerVisibility(Entry, false);
	if (Entry.Widget)
		Entry.Widget->RemoveFromParent();

	Entry.Widget = nullptr;
	Entry.bWidgetCreationAttempted = false;
}

void UWorldMapMarkerUIComponent::ClearMarkerWidgets()
{
	for (FWorldMapMarkerEntry& Entry : Markers)
		ClearMarkerWidget(Entry);
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
	ClearDestination();
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
		{
			MarkerLayer->SetDestinationVisible(false);
			MarkerLayer->SetMarkerController(nullptr);
		}

		MarkerLayer = Layer;
	}

	Layer->SetMarkerController(this);

	for (FWorldMapMarkerEntry& Entry : Markers)
		CreateMarkerWidget(Entry);
}

void UWorldMapMarkerUIComponent::DetachMarkerLayer(UWorldMapMarkerLayerWidget* Layer)
{
	if (!Layer || MarkerLayer.Get() != Layer)
		return;

	ClearMarkerWidgets();
	Layer->SetDestinationVisible(false);
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

		ClearMarkerWidget(Markers[Index]);
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
	UWorldMapMarkerLayerWidget* Layer = MarkerLayer.Get();
	if (Entry.Widget || Entry.bWidgetCreationAttempted || !Entry.Marker.IsValid() || !IsPresentationReady(Layer))
		return;

	Entry.bWidgetCreationAttempted = true;
	const TSubclassOf<UWorldMapMarkerWidget> WidgetClass = Layer->GetMarkerWidgetClass(Entry.Marker->MarkerType);
	if (!WidgetClass || WidgetClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Configure the marker WBP class for type %d in %s."), static_cast<int32>(Entry.Marker->MarkerType), *Layer->GetClass()->GetName());
		return;
	}

	Entry.Widget = CreateWidget<UWorldMapMarkerWidget>(OwningPlayerController.Get(), WidgetClass);
	if (!Entry.Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Failed to create WBP marker."));
		return;
	}

	if (!Layer->AddMarkerWidget(Entry.Widget))
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
	if (!Layer || MarkerLayer.Get() != Layer)
		return;

	if (!bPresentationStarted)
		return;

	if (!OwningPlayerController.IsValid() || !WanderingCamera.IsValid())
	{
		StopPresentation();
		return;
	}

	if (!IsPresentationReady(Layer))
	{
		ClearMarkerWidgets();
		Layer->SetDestinationVisible(false);
		return;
	}

	const FVector2D CanvasSize = CanvasGeometry.GetLocalSize();
	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
	{
		ClearMarkerWidgets();
		Layer->SetDestinationVisible(false);
		return;
	}

	APlayerController* PlayerController = OwningPlayerController.Get();
	UpdateFarView();

	const bool bLayerEnabled = bFarView || bShowMarkersInNearView;
	for (int32 Index = Markers.Num() - 1; Index >= 0; --Index)
	{
		FWorldMapMarkerEntry& Entry = Markers[Index];
		UWorldMapMarkerComponent* Marker = Entry.Marker.Get();
		if (!Marker)
		{
			ClearMarkerWidget(Entry);
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
		const bool bLocalPlayer = Marker->MarkerType != EWorldMapMarkerType::Player || MarkerOwner == PlayerController->GetPawn();
		const bool bEnabled = bLayerEnabled && bLocalPlayer && Marker->bMarkerEnabled && IsValid(MarkerOwner) && !MarkerOwner->IsHidden();
		FVector2D Position = FVector2D::ZeroVector;
		const bool bOnScreen = bEnabled && ProjectWorldToCanvas(Marker->GetComponentLocation(), CanvasGeometry, Position);
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

	UpdateDestinationMarker(CanvasGeometry);
}

void UWorldMapMarkerUIComponent::UpdateFarView()
{
	if (!WanderingCamera.IsValid())
		return;

	const float EnterDistance = FMath::Max(FarViewEnterDistance, 1.0f);
	const float ExitDistance = FMath::Clamp(FarViewExitDistance, 0.0f, EnterDistance - 1.0f);
	const float ZoomDistance = WanderingCamera->GetZoomDistance();
	if (bFarView && ZoomDistance <= ExitDistance)
		bFarView = false;
	else if (!bFarView && ZoomDistance >= EnterDistance)
		bFarView = true;
}

bool UWorldMapMarkerUIComponent::ProjectWorldToCanvas(const FVector& WorldPosition, const FGeometry& CanvasGeometry, FVector2D& OutPosition) const
{
	OutPosition = FVector2D::ZeroVector;
	if (!OwningPlayerController.IsValid())
		return false;

	const FVector2D CanvasSize = CanvasGeometry.GetLocalSize();
	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return false;

	FVector2D ScreenPosition = FVector2D::ZeroVector;
	if (!OwningPlayerController->ProjectWorldLocationToScreen(WorldPosition, ScreenPosition, false))
		return false;

	USlateBlueprintLibrary::ScreenToWidgetLocal(OwningPlayerController.Get(), CanvasGeometry, ScreenPosition, OutPosition, false);
	return OutPosition.X >= 0.0f && OutPosition.Y >= 0.0f && OutPosition.X <= CanvasSize.X && OutPosition.Y <= CanvasSize.Y;
}

void UWorldMapMarkerUIComponent::ShowDestination(const FVector& WorldPosition)
{
	if (!bPresentationStarted || WorldPosition.ContainsNaN())
		return;

	DestinationWorldPosition = WorldPosition;
	bHasDestination = true;
}

AInteractionPointBase* UWorldMapMarkerUIComponent::FindInteractionAtScreenPosition(const FVector2D& ScreenPosition) const
{
	UWorldMapMarkerLayerWidget* Layer = MarkerLayer.Get();
	if (!IsPresentationReady(Layer) || !Layer->IsVisible())
		return nullptr;

	AInteractionPointBase* BestPoint = nullptr;
	int32 BestZOrder = MIN_int32;
	int32 BestChildIndex = INDEX_NONE;
	for (const FWorldMapMarkerEntry& Entry : Markers)
	{
		UWorldMapMarkerComponent* Marker = Entry.Marker.Get();
		if (!Marker || !Marker->CanSnapDestination() || !Entry.Widget || !Entry.Widget->IsVisible())
			continue;

		const FGeometry& MarkerGeometry = Entry.Widget->GetCachedGeometry();
		const FVector2D MarkerSize = MarkerGeometry.GetLocalSize();
		if (MarkerSize.X <= 0.0f || MarkerSize.Y <= 0.0f)
			continue;

		FVector2D LocalPosition = FVector2D::ZeroVector;
		USlateBlueprintLibrary::ScreenToWidgetLocal(OwningPlayerController.Get(), MarkerGeometry, ScreenPosition, LocalPosition, false);
		if (LocalPosition.X < 0.0f || LocalPosition.Y < 0.0f || LocalPosition.X > MarkerSize.X || LocalPosition.Y > MarkerSize.Y)
			continue;

		const UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(Entry.Widget->Slot);
		const UPanelWidget* MarkerParent = Entry.Widget->GetParent();
		if (!MarkerSlot || !MarkerParent)
			continue;

		const int32 MarkerZOrder = MarkerSlot->GetZOrder();
		const int32 ChildIndex = MarkerParent->GetChildIndex(Entry.Widget);
		if (!BestPoint || MarkerZOrder > BestZOrder || (MarkerZOrder == BestZOrder && ChildIndex > BestChildIndex))
		{
			BestPoint = Cast<AInteractionPointBase>(Marker->GetOwner());
			BestZOrder = MarkerZOrder;
			BestChildIndex = ChildIndex;
		}
	}

	return BestPoint;
}

AInteractionPointBase* UWorldMapMarkerUIComponent::FindInteractionNearLocation(const FVector& WorldPosition) const
{
	if (!bPresentationStarted || !OwningPlayerController.IsValid() || WorldPosition.ContainsNaN())
		return nullptr;

	AInteractionPointBase* ClosestPoint = nullptr;
	double ClosestDistanceSquared = TNumericLimits<double>::Max();
	for (const FWorldMapMarkerEntry& Entry : Markers)
	{
		UWorldMapMarkerComponent* Marker = Entry.Marker.Get();
		if (!Marker || !Marker->CanSnapDestination())
			continue;

		const float SnapRadius = Marker->DestinationSnapRadius;
		if (!FMath::IsFinite(SnapRadius) || SnapRadius <= 0.0f)
			continue;

		AInteractionPointBase* Point = Cast<AInteractionPointBase>(Marker->GetOwner());
		const double DistanceSquared = FVector::DistSquared(WorldPosition, Point->GetActorLocation());
		if (DistanceSquared <= FMath::Square(static_cast<double>(SnapRadius)) && DistanceSquared < ClosestDistanceSquared)
		{
			ClosestPoint = Point;
			ClosestDistanceSquared = DistanceSquared;
		}
	}

	return ClosestPoint;
}

void UWorldMapMarkerUIComponent::ClearDestination()
{
	bHasDestination = false;
	DestinationWorldPosition = FVector::ZeroVector;
	if (MarkerLayer.IsValid())
		MarkerLayer->SetDestinationVisible(false);
}

void UWorldMapMarkerUIComponent::UpdateDestinationMarker(const FGeometry& CanvasGeometry)
{
	UWorldMapMarkerLayerWidget* Layer = MarkerLayer.Get();
	if (!Layer)
		return;

	if (!bHasDestination || !IsPresentationReady(Layer) || !Layer->HasDestinationMarker())
	{
		Layer->SetDestinationVisible(false);
		return;
	}

	FVector2D Position = FVector2D::ZeroVector;
	if (!ProjectWorldToCanvas(DestinationWorldPosition, CanvasGeometry, Position))
	{
		Layer->SetDestinationVisible(false);
		return;
	}

	Layer->SetDestinationPosition(Position);
	Layer->SetDestinationVisible(true);
}
