#include "StatusEffectEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectTypes.h"

void UStatusEffectEntryWidget::InitWidget(UMuksiStatusEffect* InStatusEffect, const FStatusEffectRegistryData* InRegistryData)
{
    CachedStatusEffect = InStatusEffect;

    if (!CachedStatusEffect)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::Visible);

    if (IMG_Icon)
    {
        if (InRegistryData && InRegistryData->Icon)
        {
            IMG_Icon->SetBrushFromTexture(InRegistryData->Icon);
            IMG_Icon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            IMG_Icon->SetVisibility(ESlateVisibility::Collapsed);

            UE_LOG(LogTemp, Warning,TEXT("[StatusEffectEntryWidget] Icon is null. EffectID: %s"),*CachedStatusEffect->GetEffectID().ToString());
        }
    }

    if (TXT_Stack)
    {
        const int32 Stack = CachedStatusEffect->GetCurrentStack();

        TXT_Stack->SetText(FText::AsNumber(Stack));

        TXT_Stack->SetVisibility(Stack > 1? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (TXT_Duration)
    {
        TXT_Duration->SetText(FText::AsNumber(CachedStatusEffect->GetRemainingDuration()));
    }
}