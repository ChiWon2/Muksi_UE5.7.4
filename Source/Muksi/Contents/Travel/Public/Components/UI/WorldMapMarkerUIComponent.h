#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldMapMarkerUIComponent.generated.h"

class APlayerController;
class AWanderingCameraManager;
class UWorldMapMarkerComponent;
class UWorldMapMarkerLayerWidget;
class UWorldMapMarkerWidget;
struct FGeometry;

USTRUCT()
struct FWorldMapMarkerEntry
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TWeakObjectPtr<UWorldMapMarkerComponent> Marker;

	UPROPERTY(Transient)
	TObjectPtr<UWorldMapMarkerWidget> Widget;

	bool bWidgetCreationAttempted = false;
};

UCLASS(ClassGroup = (World), meta = (BlueprintSpawnableComponent))
class MUKSI_API UWorldMapMarkerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldMapMarkerUIComponent();

	void StartPresentation(APlayerController* PlayerController, AWanderingCameraManager* CameraManager);
	void StopPresentation();
	void AttachMarkerLayer(UWorldMapMarkerLayerWidget* Layer);
	void DetachMarkerLayer(UWorldMapMarkerLayerWidget* Layer);
	void RegisterMarker(UWorldMapMarkerComponent* Marker);
	void UnregisterMarker(UWorldMapMarkerComponent* Marker);
	void UpdateMarkerLayer(UWorldMapMarkerLayerWidget* Layer, const FGeometry& CanvasGeometry);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Markers|Preview")
	bool bShowMarkersInNearView = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Markers|Presentation")
	bool bHideVisualsInFarView = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Markers|Zoom", meta = (ClampMin = "1.0", Units = "cm"))
	float FarViewEnterDistance = 12000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Markers|Zoom", meta = (ClampMin = "1.0", Units = "cm"))
	float FarViewExitDistance = 10500.0f;

private:
	void CreateMarkerWidget(FWorldMapMarkerEntry& Entry);
	void SetMarkerVisibility(FWorldMapMarkerEntry& Entry, bool bVisible);
	void ClearMarkerWidgets();
	bool IsPresentationReady() const;

	UPROPERTY(Transient)
	TArray<FWorldMapMarkerEntry> Markers;

	TWeakObjectPtr<UWorldMapMarkerLayerWidget> MarkerLayer;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<AWanderingCameraManager> WanderingCamera;
	bool bPresentationStarted = false;
	bool bFarView = false;
};
