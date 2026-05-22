#include "World/Components/TownUIControllerComponent.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/World/Data/TownDataAsset.h"
#include "Muksi/Contents/World/Zone/ZoneActor.h"
#include "MuksiFunctionLibrary.h"
#include "MuksiGameplayTags.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/World/Widget_WorldTown.h"

UTownUIControllerComponent::UTownUIControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTownUIControllerComponent::OpenTownUI(UTownDataAsset* InTownData)
{
	UE_LOG(LogTemp, Log, TEXT("OpenTownUI entered"));

	if (CurrentTownWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("OpenTownUI skipped: already open"));
		return;
	}

	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: owner is not MuksiPlayerController"));
		return;
	}

	const FZoneData ZoneData = PC->GetCurrentZoneData();
	if (!ZoneData.bCanOpenTownUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI blocked: current zone does not allow Town UI"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(PC);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: UISubsystem is null"));
		return;
	}

	TSoftClassPtr<UWidget_ActivatableBase> ResolvedTownWidgetClass = TownWidgetClass;
	if (ResolvedTownWidgetClass.IsNull())
	{
		ResolvedTownWidgetClass = UMuksiFunctionLibrary::GetMuksiSoftWidgetClassByTag(
			MuksiGameplayTag::Muksi_Widget_World_Town
		);
	}

	if (ResolvedTownWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: TownWidgetClass is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ResolvedTownWidgetClass,
		true,
		[this, InTownData](UWidget_ActivatableBase* CreatedWidget)
		{
			if (!CreatedWidget)
			{
				return;
			}

			CurrentTownWidget = CreatedWidget;

			if (UWidget_WorldTown* TownWidget = Cast<UWidget_WorldTown>(CreatedWidget))
			{
				TownWidget->InitializeTown(InTownData);
				UE_LOG(LogTemp, Warning, TEXT("Town UI initialized with DataAsset: %s"), *GetNameSafe(InTownData));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CreatedWidget is not UWidget_WorldTown: %s"), *GetNameSafe(CreatedWidget));
			}

			UE_LOG(LogTemp, Log, TEXT("Town UI created: %s"), *GetNameSafe(CreatedWidget));
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("Town UI pushed: %s"), *GetNameSafe(PushedWidget));
		}
	);
}

void UTownUIControllerComponent::CloseTownUI() 
{
	if (!CurrentTownWidget)
	{
		return;
	}

	UWidget_ActivatableBase* TownWidget = CurrentTownWidget;
	CurrentTownWidget = nullptr;

	TownWidget->DeactivateWidget();
}

