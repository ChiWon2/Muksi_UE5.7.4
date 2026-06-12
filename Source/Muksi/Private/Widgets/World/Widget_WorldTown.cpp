#include "Widgets/World/Widget_WorldTown.h"

#include "Widgets/World/Widget_Shop.h"
#include "Widgets/World/Widget_TownInteractionButton.h"
#include "Muksi/Contents/World/Data/TownDataAsset.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "MuksiGameplayTags.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "CommonTextBlock.h"
#include "Controllers/MuksiPlayerController.h"
#include "World/Components/TownUIControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldUIManager.h"

void UWidget_WorldTown::InitializeTown(UTownDataAsset* InTownData)
{
	CurrentTownData = InTownData;
	RefreshTown();
}


void UWidget_WorldTown::NativeOnActivated()
{
	Super::NativeOnActivated();

	RequestRefreshFocus();

	if (CloseButton)
	{
		CloseButton->SetFocus();
		CloseButton->OnClicked.RemoveAll(this);
		CloseButton->OnClicked.AddDynamic(this, &UWidget_WorldTown::HandleCloseButtonClicked);
	}
}

void UWidget_WorldTown::RefreshTown()
{
	if (!CurrentTownData)
	{
		if (TownNameText)
		{
			TownNameText->SetText(FText::FromString(TEXT("Unknown Town")));
		}

		if (DescriptionText)
		{
			DescriptionText->SetText(FText::GetEmpty());
		}

		if (InteractionListPanel)
		{
			InteractionListPanel->ClearChildren();
		}

		return;
	}

	if (TownNameText)
	{
		TownNameText->SetText(CurrentTownData->TownName);
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(CurrentTownData->Description);
	}

	if (ThumbnailImage)
	{
		ThumbnailImage->SetBrushFromTexture(CurrentTownData->Thumbnail);
	}

	RebuildInteractionButtons();
}

void UWidget_WorldTown::RebuildInteractionButtons()
{
	if (!InteractionListPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Town UI failed: InteractionListPanel is null"));
		return;
	}

	InteractionListPanel->ClearChildren();

	if (!CurrentTownData || !InteractionButtonWidgetClass)
	{
		return;
	}

	for (const FTownInteractionData& InteractionData : CurrentTownData->InteractionList)
	{
		if (!InteractionData.bUnlockedByDefault)
		{
			continue;
		}

		UWidget_TownInteractionButton* ButtonWidget =
			CreateWidget<UWidget_TownInteractionButton>(GetOwningPlayer(), InteractionButtonWidgetClass);

		if (!ButtonWidget)
		{
			continue;
		}

		ButtonWidget->InitializeInteraction(InteractionData);
		ButtonWidget->OnTownInteractionClicked.AddUObject(
			this,
			&UWidget_WorldTown::HandleInteractionClicked
		);

		InteractionListPanel->AddChild(ButtonWidget);
	}
}

void UWidget_WorldTown::HandleInteractionClicked(const FTownInteractionData& InteractionData)
{
	const FString TypeName = StaticEnum<ETownInteractionType>()
		? StaticEnum<ETownInteractionType>()->GetNameStringByValue(static_cast<int64>(InteractionData.InteractionType))
		: TEXT("Unknown");

	UE_LOG(LogTemp, Log, TEXT("[TownInteraction] Clicked. Town=%s InteractionId=%s DisplayName=%s Type=%s"),
		*GetNameSafe(CurrentTownData),
		*InteractionData.InteractionId.ToString(),
		*InteractionData.DisplayName.ToString(),
		*TypeName);

	switch (InteractionData.InteractionType)
	{
	case ETownInteractionType::TrainingGrounds:
		OpenTrainingGrounds(InteractionData);
		break;

	case ETownInteractionType::Forge:
		OpenForge(InteractionData);
		break;

	case ETownInteractionType::Shrine:
		OpenShrine(InteractionData);
		break;

	case ETownInteractionType::TangClan:
		OpenTangClan(InteractionData);
		break;

	case ETownInteractionType::Custom:
		OpenCustomInteraction(InteractionData);
		break;

	case ETownInteractionType::Shop:
		OpenShop(InteractionData);
		break;


	default:
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] Unsupported interaction type. Id=%s"),
			*InteractionData.InteractionId.ToString());
		break;
	}
}

void UWidget_WorldTown::OpenTrainingGrounds(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[TownInteraction] OpenTrainingGrounds entry. Id=%s"),
		*InteractionData.InteractionId.ToString());

	if (TrainingGroundsWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Open failed: TrainingGroundsWidgetClass is null"));
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Open failed: PlayerController is null"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Open failed: UISubsystem is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		TrainingGroundsWidgetClass,
		true,
		[](UWidget_ActivatableBase* CreatedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] CreatedWidget=%s"),
				*GetNameSafe(CreatedWidget));
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] PushedWidget=%s"),
				*GetNameSafe(PushedWidget));
		}
	);
}

void UWidget_WorldTown::OpenForge(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[TownInteraction] OpenForge entry. Id=%s"),
		*InteractionData.InteractionId.ToString());

	if (ForgeWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] OpenForge failed: ForgeWidgetClass is null"));
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] OpenForge failed: PlayerController is null"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] OpenForge failed: UISubsystem is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ForgeWidgetClass,
		true,
		[](UWidget_ActivatableBase* CreatedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Forge] CreatedWidget=%s"),
				*GetNameSafe(CreatedWidget));
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Forge] PushedWidget=%s"),
				*GetNameSafe(PushedWidget));
		}
	);
}

void UWidget_WorldTown::OpenShrine(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[TownInteraction] OpenShrine entry. Id=%s"),
		*InteractionData.InteractionId.ToString());

	if (ShrineWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Open failed: ShrineWidgetClass is null"));
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Open failed: PlayerController is null"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Open failed: UISubsystem is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ShrineWidgetClass,
		true,
		[](UWidget_ActivatableBase* CreatedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Shrine] CreatedWidget=%s"),
				*GetNameSafe(CreatedWidget));
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Shrine] PushedWidget=%s"),
				*GetNameSafe(PushedWidget));
		}
	);
}

void UWidget_WorldTown::OpenTangClan(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[TangClan] Opened"));

	if (TangClanWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TangClan] Open failed: TangClanWidgetClass is null"));
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TangClan] Open failed: PlayerController is null"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TangClan] Open failed: UISubsystem is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		TangClanWidgetClass,
		true,
		[](UWidget_ActivatableBase* CreatedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[TangClan] CreatedWidget=%s"),
				*GetNameSafe(CreatedWidget));
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[TangClan] PushedWidget=%s"),
				*GetNameSafe(PushedWidget));
		}
	);
}

void UWidget_WorldTown::OpenShop(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[Shop] Opened"));

	if (ShopWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shop] Open failed: ShopClass is null"));
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shop] Open failed: PlayerController is null"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shop] Open failed: UISubsystem is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ShopWidgetClass,
		true,
		[InteractionData](UWidget_ActivatableBase* CreatedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Shop] CreatedWidget=%s"),
				*GetNameSafe(CreatedWidget));

			if (UWidget_Shop* ShopWidget = Cast<UWidget_Shop>(CreatedWidget))
			{
				ShopWidget->SetShopData(InteractionData.ShopData);
			}
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[Shop] PushedWidget=%s"),
				*GetNameSafe(PushedWidget));
		}
	);
}

void UWidget_WorldTown::OpenCustomInteraction(const FTownInteractionData& InteractionData)
{
	UE_LOG(LogTemp, Log, TEXT("[TownInteraction] OpenCustomInteraction entry. Id=%s"),
		*InteractionData.InteractionId.ToString());

	// TODO: Later route by InteractionId, GameplayTag, or custom data.
}


void UWidget_WorldTown::HandleCloseButtonClicked()
{
	AWorldUIManager* WorldUIManager = Cast<AWorldUIManager>(
		UGameplayStatics::GetActorOfClass(this, AWorldUIManager::StaticClass())
	);

	if (WorldUIManager)
	{
		WorldUIManager->CloseTownUI();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HandleCloseButtonClicked: WorldUIManager not found. Fallback DeactivateWidget."));
	DeactivateWidget();
}