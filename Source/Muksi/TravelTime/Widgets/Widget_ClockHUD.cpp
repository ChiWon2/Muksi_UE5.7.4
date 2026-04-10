#include "Widget_ClockHUD.h"
#include "Components/TextBlock.h"
#include "../TravelTimeSubsystem.h"

void UWidget_ClockHUD::NativeOnActivated()
{
    Super::NativeOnActivated();
    TravelTimeSubsystem = UTravelTimeSubsystem::Get(this);
    UpdateTime();
}

void UWidget_ClockHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdateTime();
}



void UWidget_ClockHUD::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    bIsHovered = true;
}

void UWidget_ClockHUD::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    bIsHovered = false;
}

void UWidget_ClockHUD::UpdateTime()
{
    if (!TravelTimeSubsystem || !TXT_TimeText)
        return;
    TXT_TimeText->SetText(TravelTimeSubsystem->GetCurrentFormattedTime(bIsHovered));
}