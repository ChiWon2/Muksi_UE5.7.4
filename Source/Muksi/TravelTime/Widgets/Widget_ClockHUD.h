#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "../TravelTimeTypes.h"
#include "Widget_ClockHUD.generated.h"

class UTextBlock;
class UTravelTimeSubsystem;


UCLASS()
class MUKSI_API UWidget_ClockHUD : public UWidget_ActivatableBase
{
    GENERATED_BODY()

protected:
    virtual void NativeOnActivated() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_TimeText;

    UPROPERTY()
    UTravelTimeSubsystem* TravelTimeSubsystem;

    bool bIsHovered = false;

private:
    void UpdateTime();
};