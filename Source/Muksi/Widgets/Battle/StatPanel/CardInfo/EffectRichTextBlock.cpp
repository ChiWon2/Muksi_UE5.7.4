// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectRegistry.h"
#include "Muksi/Contents/Battle/StatusEffect/StatusEffectDefinitionDataAsset.h"
#include "Muksi/Widgets/Battle/Popup/EffectDescriptionPopup.h"


void UEffectRichTextBlock::ShowEffectDescriptionByHover(FName EffectID, const FEffectKeywordStyle& EffectStyle)
{
	UStatusEffectDefinitionDataAsset* Definition =
		FindStatusEffectDefinition(EffectID);

	if (!Definition)
	{
		return;
	}

	EnsureEffectDescriptionPopup();

	if (!EffectDescriptionPopup)
	{
		return;
	}

	const FVector2D MousePosition =
		UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	EffectDescriptionPopup->ShowByHover(
		EffectID,
		Definition->DisplayName,
		Definition->Description,
		EffectStyle.IconBrush,
		MousePosition
	);
}

void UEffectRichTextBlock::HideEffectDescriptionByHover()
{
	if (!EffectDescriptionPopup)
	{
		return;
	}

	EffectDescriptionPopup->HideByHover();
}

void UEffectRichTextBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	if (EffectDescriptionPopup)
	{
		EffectDescriptionPopup->RemoveFromParent();
		EffectDescriptionPopup = nullptr;
	}
	Super::ReleaseSlateResources(bReleaseChildren);
}




UStatusEffectDefinitionDataAsset* UEffectRichTextBlock::FindStatusEffectDefinition(FName EffectID) const
{
	UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);

	if (!ManagerSubsystem)
	{
		return nullptr;
	}

	ABattleManager* BattleManager = ManagerSubsystem->GetManager<ABattleManager>();

	if (!BattleManager)
	{
		return nullptr;
	}

	UMuksiStatusEffectRegistry* Registry = BattleManager->GetStatusEffectRegistry();

	if (!Registry)
	{
		return nullptr;
	}

	return Registry->FindDefinition(EffectID);
}

void UEffectRichTextBlock::EnsureEffectDescriptionPopup()
{
	if (EffectDescriptionPopup)
	{
		return;
	}

	if (!EffectDescriptionPopupClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"[EffectRichTextBlock] "
				"EffectDescriptionPopupClass is null."
			)
		);

		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();

	if (!PlayerController)
	{
		return;
	}

	EffectDescriptionPopup = CreateWidget<UEffectDescriptionPopup>(PlayerController, EffectDescriptionPopupClass);

	if (!EffectDescriptionPopup)
	{
		return;
	}

	EffectDescriptionPopup->AddToViewport(1000);
}
