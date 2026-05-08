#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_WorldMainScreen.generated.h"

UCLASS()
class MUKSI_API UWidget_WorldMainScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DefaultFocusWidget;
};
