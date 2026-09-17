#include "StatusEffectBarWidget.h"

#include "Components/HorizontalBox.h"

#include "StatusEffectEntryWidget.h"

#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/StatusEffectDefinitionDataAsset.h"

void UStatusEffectBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindObservedComponent();
	Refresh();
}

void UStatusEffectBarWidget::NativeDestruct()
{
	UnbindObservedComponent();
	Super::NativeDestruct();
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
		if (!EntryWidget)
			continue;
		UStatusEffectDefinitionDataAsset* Definition = ObservedStatusEffectComponent->FindStatusEffectDefinition(Effect->GetEffectID());
		EntryWidget->InitWidget(Effect, Definition);
		HB_StatusEffects->AddChild(EntryWidget);
	}
}
