#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "WorldMapMarkerComponent.generated.h"

class UMeshComponent;
class UTexture2D;
class UWorldMapMarkerUIComponent;

UENUM(BlueprintType)
enum class EWorldMapMarkerType : uint8
{
	Interaction,
	Player
};

UCLASS(ClassGroup = (World), meta = (BlueprintSpawnableComponent))
class MUKSI_API UWorldMapMarkerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWorldMapMarkerComponent();

	UFUNCTION(BlueprintPure, Category = "World Map Marker")
	FText GetDisplayName() const;

	void AddVisualComponent(UMeshComponent* VisualComponent);
	void SetWorldVisualsHidden(bool bHidden);
	void SetMarkerUIController(UWorldMapMarkerUIComponent* Controller);
	bool CanSnapDestination() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker")
	bool bMarkerEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Marker")
	EWorldMapMarkerType MarkerType = EWorldMapMarkerType::Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker")
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker")
	FText DisplayNameOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker")
	FLinearColor IconTint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker|Size", meta = (ClampMin = "8.0", ClampMax = "128.0"))
	float IconSize = 28.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker|Size", meta = (ClampMin = "8", ClampMax = "48"))
	int32 NameFontSize = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker")
	bool bShowName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker", meta = (DisplayName = "Z Order"))
	int32 SortPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker|Destination")
	bool bEnableDestinationSnap = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Map Marker|Destination", meta = (ClampMin = "0.0", Units = "cm"))
	float DestinationSnapRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = "World Map Marker|Visuals", meta = (UseComponentPicker, AllowedClasses = "/Script/Engine.MeshComponent"))
	TArray<FComponentReference> AdditionalVisualComponents;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	struct FVisualState
	{
		TWeakObjectPtr<UMeshComponent> Component;
		bool bWasVisible = true;
	};

	TArray<TWeakObjectPtr<UMeshComponent>> VisualComponents;
	TArray<FVisualState> HiddenVisualStates;
	TWeakObjectPtr<UWorldMapMarkerUIComponent> MarkerUIController;
	bool bWorldVisualsHidden = false;
};
