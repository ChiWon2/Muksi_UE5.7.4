#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_TangClan.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class MUKSI_API UWidget_TangClan : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void HandleBackButtonClicked();
};
