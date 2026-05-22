#include "Widgets/World/Widget_WorldMainScreen.h"

void UWidget_WorldMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("WorldMainScreen activated"));
}

FReply UWidget_WorldMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("WorldMainScreen MouseDown -> Unhandled"));
	return FReply::Unhandled();
}