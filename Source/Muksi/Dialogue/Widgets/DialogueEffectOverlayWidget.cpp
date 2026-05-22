#include "DialogueEffectOverlayWidget.h"
#include "Components/Image.h"

void UDialogueEffectOverlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Hidden);

    FWidgetAnimationDynamicEvent AnimFinishedEvent;

    AnimFinishedEvent.BindDynamic(this, &UDialogueEffectOverlayWidget::HandleFadeAnimationFinished);

    BindToAnimationFinished(FadeAnim, AnimFinishedEvent);
}

void UDialogueEffectOverlayWidget::PlayEffect(const FDialogueEffectPreset& Preset)
{
    if (!FadeImage || !FadeAnim)
        return;

    SetVisibility(ESlateVisibility::Visible);

    FadeImage->SetColorAndOpacity(Preset.FadeColor);

    PlayAnimation(FadeAnim,0.f,1,EUMGSequencePlayMode::Forward,1.f / Preset.FadeInTime);
}

void UDialogueEffectOverlayWidget::HandleFadeAnimationFinished()
{
    SetVisibility(ESlateVisibility::Hidden);
}