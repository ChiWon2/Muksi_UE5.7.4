#include "StatusEffectBarWidget.h"

#include "Components/HorizontalBox.h"

#include "StatusEffectEntryWidget.h"

#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectSubsystem.h"

void UStatusEffectBarWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    BindObservedComponent();
    if (ObservedStatusEffectComponent)
    {
        Refresh();
    }
}

void UStatusEffectBarWidget::NativeOnDeactivated()
{
    UnbindObservedComponent();

    Super::NativeOnDeactivated();
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
    {
        return;
    }

    ObservedStatusEffectComponent->OnStatusEffectsChanged.RemoveAll(this);

    ObservedStatusEffectComponent->OnStatusEffectsChanged.AddUObject(this,&UStatusEffectBarWidget::HandleStatusEffectsChanged);
}

void UStatusEffectBarWidget::UnbindObservedComponent()
{
    if (!ObservedStatusEffectComponent)
    {
        return;
    }

    ObservedStatusEffectComponent->OnStatusEffectsChanged.RemoveAll(this);
}

void UStatusEffectBarWidget::HandleStatusEffectsChanged()
{
    Refresh();
}

void UStatusEffectBarWidget::Refresh()
{
    if (!HB_StatusEffects)
    {
        return;
    }

    HB_StatusEffects->ClearChildren();

    if (!ObservedStatusEffectComponent)
    {
        return;
    }

    if (!StatusEffectEntryWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectBarWidget] StatusEffectEntryWidgetClass is nullptr."));
        return;
    }

    UMuksiStatusEffectSubsystem* StatusEffectSubsystem = UMuksiStatusEffectSubsystem::Get(this);
    if (!StatusEffectSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectBarWidget] Cannot find StatusEffectSubsystem."));
        return;
    }

    const TArray<TObjectPtr<UMuksiStatusEffect>>& ActiveEffects = ObservedStatusEffectComponent->GetActiveEffects();

    for (UMuksiStatusEffect* Effect : ActiveEffects)
    {
        if (!Effect)
        {
            continue;
        }

        UStatusEffectEntryWidget* EntryWidget = CreateWidget<UStatusEffectEntryWidget>(this, StatusEffectEntryWidgetClass);

        if (!EntryWidget)
        {
            continue;
        }

        const FStatusEffectRegistryData* RegistryData = StatusEffectSubsystem->FindRegistryData(Effect->GetEffectID());

        EntryWidget->InitWidget(Effect, RegistryData);

        HB_StatusEffects->AddChild(EntryWidget);
    }
}