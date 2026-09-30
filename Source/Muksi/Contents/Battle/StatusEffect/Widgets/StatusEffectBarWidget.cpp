#include "StatusEffectBarWidget.h"

#include "Components/HorizontalBox.h"

#include "StatusEffectEntryWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/StatusEffectDefinitionDataAsset.h"
#include "Muksi/Widgets/Battle/Popup/EffectDescriptionPopup.h"

void UStatusEffectBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindObservedComponent();
	Refresh();
}

void UStatusEffectBarWidget::NativeDestruct()
{
	UnbindObservedComponent();

	if (EffectDescriptionPopup)
	{
		EffectDescriptionPopup->RemoveFromParent();
		EffectDescriptionPopup = nullptr;
	}
	Super::NativeDestruct();
}


void UStatusEffectBarWidget::HandleStatusEffectEntryUnhovered()
{
	if (!EffectDescriptionPopup)
	{
		return;
	}

	EffectDescriptionPopup->HideByHover();
}

void UStatusEffectBarWidget::EnsureEffectDescriptionPopup()
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
			TEXT("[StatusEffectBar] EffectDescriptionPopupClass is null.")
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


void UStatusEffectBarWidget::InitWidget(UMuksiStatusEffectComponent* InStatusEffectComponent)
{
	if (ObservedStatusEffectComponent == InStatusEffectComponent)
	{
		Refresh();
		return;
	}

	UnbindObservedComponent();
	ObservedStatusEffectComponent = InStatusEffectComponent;
	BindObservedComponent();
	Refresh();
}

void UStatusEffectBarWidget::BindObservedComponent()
{
	if (!ObservedStatusEffectComponent)
		return;
	ObservedStatusEffectComponent->OnStatusEffectsChanged.RemoveAll(this);
	ObservedStatusEffectComponent->OnStatusEffectsChanged.AddUObject(this, &UStatusEffectBarWidget::HandleStatusEffectsChanged);
}

void UStatusEffectBarWidget::UnbindObservedComponent()
{
	if (!ObservedStatusEffectComponent)
		return;
	ObservedStatusEffectComponent->OnStatusEffectsChanged.RemoveAll(this);
}

void UStatusEffectBarWidget::HandleStatusEffectsChanged()
{
	Refresh();
}


void UStatusEffectBarWidget::HandleStatusEffectEntryHovered(UMuksiStatusEffect* StatusEffect,
	UStatusEffectDefinitionDataAsset* Definition)
{
	if (!StatusEffect || !Definition)
	{
		return;
	}
	
	EnsureEffectDescriptionPopup();
	
	if (!EffectDescriptionPopup)
	{
		return;
	}

	FSlateBrush IconBrush;

	if (UTexture2D* IconTexture =
		Definition->Icon.LoadSynchronous())
	{
		IconBrush.SetResourceObject(IconTexture);
		IconBrush.DrawAs =
			ESlateBrushDrawType::Image;
	}

	const FVector2D MousePosition =
		UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	EffectDescriptionPopup->ShowByHover(
		StatusEffect->GetEffectID(),
		Definition->DisplayName,
		Definition->Description,
		IconBrush,
		MousePosition
	);
}


void UStatusEffectBarWidget::Refresh()
{
	if (!HB_StatusEffects)
		return;
	HB_StatusEffects->ClearChildren();
	if (!ObservedStatusEffectComponent)
		return;
	if (!StatusEffectEntryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[StatusEffectBarWidget] StatusEffectEntryWidgetClass is nullptr."));
		return;
	}
	const TArray<TObjectPtr<UMuksiStatusEffect>>& ActiveEffects = ObservedStatusEffectComponent->GetActiveEffects();
	for (UMuksiStatusEffect* Effect : ActiveEffects)
	{
		if (!Effect)
			continue;
		UStatusEffectEntryWidget* EntryWidget = CreateWidget<UStatusEffectEntryWidget>(this, StatusEffectEntryWidgetClass);
		
		if (!EntryWidget)continue;
		
		UStatusEffectDefinitionDataAsset* Definition = ObservedStatusEffectComponent->FindStatusEffectDefinition(Effect->GetEffectID());
		
		EntryWidget->InitWidget(Effect, Definition);
		EntryWidget->OnStatusEffectEntryHovered.AddUObject(this, &UStatusEffectBarWidget::HandleStatusEffectEntryHovered);
		EntryWidget->OnStatusEffectEntryUnhovered.AddUObject(this, &UStatusEffectBarWidget::HandleStatusEffectEntryUnhovered);
		HB_StatusEffects->AddChild(EntryWidget);
	}
}

