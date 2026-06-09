#include "TabButton.h"

#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

#include "Kismet/GameplayStatics.h"


void UTabButton::NativeConstruct()
{
    Super::NativeConstruct();

    BTN_Tab->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleClicked);
}

void UTabButton::NativeDestruct()
{
    BTN_Tab->OnClicked.RemoveDynamic(this,&ThisClass::HandleClicked);

    Super::NativeDestruct();
}

void UTabButton::HandleClicked()
{
    OnTabClicked.Broadcast(this);
}

void UTabButton::SetTabText(const FText& InText)
{
    TXT_TabName->SetText(InText);
}

void UTabButton::SetSelected(bool bInSelected)
{
    if (bInSelected)
    {
        Border_Background->SetBrushColor(
            FLinearColor(0.2f, 0.5f, 1.f, 1.f)
        );
    }
    else
    {
        Border_Background->SetBrushColor(
            FLinearColor(0.1f, 0.1f, 0.1f, 1.f)
        );
    }
}