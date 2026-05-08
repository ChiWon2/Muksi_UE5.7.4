#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_Shrine.generated.h"

class UButton;
class UStatComponent;

UCLASS()
class MUKSI_API UWidget_Shrine : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HealButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RecoverInternalEnergyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void HandleHealButtonClicked();

	UFUNCTION()
	void HandleRecoverInternalEnergyButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();

private:
	UStatComponent* GetPlayerStatComponent() const;
};
