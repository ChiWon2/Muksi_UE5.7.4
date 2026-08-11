#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectBarWidget.generated.h"

class UHorizontalBox;
class UStatusEffectEntryWidget;
class UMuksiStatusEffectComponent;

UCLASS()
class MUKSI_API UStatusEffectBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWidget(UMuksiStatusEffectComponent* InStatusEffectComponent);
	void Refresh();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HB_StatusEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Status Effect")
	TSubclassOf<UStatusEffectEntryWidget> StatusEffectEntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UMuksiStatusEffectComponent> ObservedStatusEffectComponent;

private:
	void BindObservedComponent();
	void UnbindObservedComponent();
	void HandleStatusEffectsChanged();
};
