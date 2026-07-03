#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "StatusEffectBarWidget.generated.h"

class UHorizontalBox;
class UStatusEffectEntryWidget;
class UMuksiStatusEffectComponent;
class UMuksiStatusEffectSubsystem;

UCLASS()
class MUKSI_API UStatusEffectBarWidget : public UWidget_ActivatableBase
{
    GENERATED_BODY()

public:
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

public:
    UFUNCTION(BlueprintCallable)
    void InitWidget(UMuksiStatusEffectComponent* InStatusEffectComponent);
    void Refresh();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> HB_StatusEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Status Effect")
    TSubclassOf<UStatusEffectEntryWidget> StatusEffectEntryWidgetClass;

protected:
    UPROPERTY()
    TObjectPtr<UMuksiStatusEffectComponent> ObservedStatusEffectComponent;

private:
    void BindObservedComponent();
    void UnbindObservedComponent();

    void HandleStatusEffectsChanged();
};