#include "Widget_ClockHUD.h"
#include "Components/TextBlock.h"
#include "../TravelTimeSubsystem.h"

void UWidget_ClockHUD::NativeConstruct()
{
    Super::NativeConstruct();

    TravelTimeSubsystem = UTravelTimeSubsystem::Get(this);

    if (TravelTimeSubsystem)
    {
        TravelTimeSubsystem->OnHourPassed.AddUniqueDynamic(this, &UWidget_ClockHUD::HandleHourPassed);
        TravelTimeSubsystem->OnMinuteChanged.AddUniqueDynamic(this, &UWidget_ClockHUD::HandleMinuteChanged);
    }

    UpdateTime();
}

void UWidget_ClockHUD::NativeDestruct()
{
    if (TravelTimeSubsystem)
    {
        TravelTimeSubsystem->OnHourPassed.RemoveDynamic(this, &UWidget_ClockHUD::HandleHourPassed);
        TravelTimeSubsystem->OnMinuteChanged.RemoveDynamic(this, &UWidget_ClockHUD::HandleMinuteChanged);
    }

    TravelTimeSubsystem = nullptr;

    Super::NativeDestruct();
}

void UWidget_ClockHUD::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    bIsHovered = true;
    UpdateTime();
}

void UWidget_ClockHUD::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    bIsHovered = false;
    UpdateTime();
}

void UWidget_ClockHUD::HandleHourPassed(FTravelDate CurrentTime)
{
    UpdateTime();
}

void UWidget_ClockHUD::HandleMinuteChanged()
{
    if (bIsHovered)
        UpdateTime();
}

void UWidget_ClockHUD::UpdateTime()
{
    if (!TravelTimeSubsystem || !TXT_TimeText)
        return;

    TXT_TimeText->SetText(TravelTimeSubsystem->GetCurrentFormattedTime(bIsHovered));
}
