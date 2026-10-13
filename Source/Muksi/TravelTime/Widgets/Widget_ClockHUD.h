#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../TravelTimeTypes.h"
#include "Widget_ClockHUD.generated.h"

class UTextBlock;
class UTravelTimeSubsystem;

UCLASS()
class MUKSI_API UWidget_ClockHUD : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_TimeText;

    UPROPERTY()
    TObjectPtr<UTravelTimeSubsystem> TravelTimeSubsystem;

    bool bIsHovered = false;

    UFUNCTION()
    void HandleHourPassed(FTravelDate CurrentTime);

    UFUNCTION()
    void HandleMinuteChanged();

    void UpdateTime();
};
