#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Effect/DialogueEffectTypes.h"
#include "DialogueEffectOverlayWidget.generated.h"

class UImage;

UCLASS()
class MUKSI_API UDialogueEffectOverlayWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    void PlayEffect(const FDialogueEffectPreset& Preset);

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> FadeImage;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeAnim;

    UFUNCTION()
    void HandleFadeAnimationFinished();
};