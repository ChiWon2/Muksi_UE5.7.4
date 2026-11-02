#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"

#include "Muksi/Contents/Travel/Public/Components/UI/WorldMapMarkerUIComponent.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"

#include "Components/MeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UWorldMapMarkerComponent::UWorldMapMarkerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWorldMapMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	AWorldUIManager* UIManager = Cast<AWorldUIManager>(UGameplayStatics::GetActorOfClass(this, AWorldUIManager::StaticClass()));
	if (UIManager && UIManager->GetWorldMapMarkerUIComponent())
		UIManager->GetWorldMapMarkerUIComponent()->RegisterMarker(this);
}

void UWorldMapMarkerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MarkerUIController.IsValid())
		MarkerUIController->UnregisterMarker(this);

	SetWorldVisualsHidden(false);
	Super::EndPlay(EndPlayReason);
}

FText UWorldMapMarkerComponent::GetDisplayName() const
{
	if (!DisplayNameOverride.IsEmpty())
		return DisplayNameOverride;

	if (const AInteractionPointBase* Point = Cast<AInteractionPointBase>(GetOwner()))
		return Point->GetWorldMapDisplayName();

	if (Cast<ACharacter>(GetOwner()))
		return NSLOCTEXT("WorldMapMarker", "Player", "플레이어");

	return NSLOCTEXT("WorldMapMarker", "Location", "거점");
}

void UWorldMapMarkerComponent::SetMarkerUIController(UWorldMapMarkerUIComponent* Controller)
{
	MarkerUIController = Controller;
}

void UWorldMapMarkerComponent::AddVisualComponent(UMeshComponent* VisualComponent)
{
	if (!IsValid(VisualComponent) || VisualComponent->GetOwner() != GetOwner())
		return;

	if (VisualComponents.Contains(VisualComponent))
		return;

	VisualComponents.Add(VisualComponent);

	if (bWorldVisualsHidden)
	{
		FVisualState State;
		State.Component = VisualComponent;
		State.bWasVisible = VisualComponent->IsVisible();
		HiddenVisualStates.Add(State);
		VisualComponent->SetVisibility(false, false);
	}
}

void UWorldMapMarkerComponent::SetWorldVisualsHidden(bool bHidden)
{
	if (bWorldVisualsHidden == bHidden)
		return;

	if (bHidden)
	{
		for (const FComponentReference& Reference : AdditionalVisualComponents)
			AddVisualComponent(Cast<UMeshComponent>(Reference.GetComponent(GetOwner())));

		HiddenVisualStates.Reset();

		for (const TWeakObjectPtr<UMeshComponent>& Visual : VisualComponents)
		{
			if (!Visual.IsValid())
				continue;

			FVisualState State;
			State.Component = Visual;
			State.bWasVisible = Visual->IsVisible();
			HiddenVisualStates.Add(State);
			Visual->SetVisibility(false, false);
		}
	}
	else
	{
		for (const FVisualState& State : HiddenVisualStates)
		{
			if (State.Component.IsValid())
				State.Component->SetVisibility(State.bWasVisible, false);
		}

		HiddenVisualStates.Reset();
	}

	bWorldVisualsHidden = bHidden;
}
