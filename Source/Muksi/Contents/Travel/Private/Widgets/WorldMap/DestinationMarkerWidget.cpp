#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/DestinationMarkerWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"

void UDestinationMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetMarkerVisible(false);

	if (!IsConfigured())
		UE_LOG(LogTemp, Warning, TEXT("[DestinationMarker] Add OuterImage and CenterImage in %s."), *GetClass()->GetName());
}

void UDestinationMarkerWidget::NativeDestruct()
{
	SetMarkerVisible(false);
	Super::NativeDestruct();
}

bool UDestinationMarkerWidget::IsConfigured() const
{
	return OuterImage && CenterImage;
}

void UDestinationMarkerWidget::SetMarkerVisible(bool bVisible)
{
	bVisible = bVisible && IsConfigured();
	SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (bMarkerVisible == bVisible)
		return;

	bMarkerVisible = bVisible;
	if (!OuterPulseAnimation)
		return;

	if (bVisible)
		PlayAnimation(OuterPulseAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f, true);
	else
		StopAnimation(OuterPulseAnimation);
}
