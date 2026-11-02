#include "Muksi/Contents/Travel/Public/Widgets/WorldMap/WorldMapMarkerWidget.h"

#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UWorldMapMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!HasValidMarkerLayout())
	{
		UE_LOG(LogTemp, Warning, TEXT("[WorldMapMarker] Invalid WBP layout: %s. Root must be MarkerContent; IconAnchor and NameContainer must use Auto HorizontalBox slots."), *GetClass()->GetName());
		return;
	}

	NameContainer->SetFlowDirectionPreference(EFlowDirectionPreference::Culture);
	SetNameOnLeft(false);
}

bool UWorldMapMarkerWidget::HasValidMarkerLayout() const
{
	if (!MarkerContent || GetRootWidget() != MarkerContent || !IconAnchor || !IconImage || !NameContainer || !NameText)
		return false;

	if (IconAnchor->GetParent() != MarkerContent || NameContainer->GetParent() != MarkerContent)
		return false;

	for (int32 ChildIndex = 0; ChildIndex < MarkerContent->GetChildrenCount(); ++ChildIndex)
	{
		const UWidget* Child = MarkerContent->GetChildAt(ChildIndex);
		const UHorizontalBoxSlot* ChildSlot = Cast<UHorizontalBoxSlot>(Child->Slot);
		if (!ChildSlot || ChildSlot->GetSize().SizeRule != ESlateSizeRule::Automatic)
			return false;
	}

	return true;
}

void UWorldMapMarkerWidget::UpdateMarkerAppearance(const UWorldMapMarkerComponent* Marker)
{
	if (!Marker || !HasValidMarkerLayout())
		return;

	const FText DisplayName = Marker->GetDisplayName();
	bool bLayoutChanged = false;
	const float IconExtent = FMath::Clamp(Marker->IconSize, 8.0f, 128.0f);
	if (!IconAnchor->IsWidthOverride() || !IconAnchor->IsHeightOverride()
		|| !FMath::IsNearlyEqual(IconAnchor->GetWidthOverride(), IconExtent) || !FMath::IsNearlyEqual(IconAnchor->GetHeightOverride(), IconExtent))
	{
		IconAnchor->SetWidthOverride(IconExtent);
		IconAnchor->SetHeightOverride(IconExtent);
		bLayoutChanged = true;
	}

	FSlateFontInfo Font = NameText->GetFont();
	const int32 FontSize = FMath::Clamp(Marker->NameFontSize, 8, 48);
	if (Font.Size != FontSize)
	{
		Font.Size = FontSize;
		NameText->SetFont(Font);
		bLayoutChanged = true;
	}

	if (!NameText->GetText().EqualTo(DisplayName))
	{
		NameText->SetText(DisplayName);
		bLayoutChanged = true;
	}

	const ESlateVisibility NameVisibility = Marker->bShowName && !DisplayName.IsEmpty() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
	if (NameContainer->GetVisibility() != NameVisibility)
	{
		NameContainer->SetVisibility(NameVisibility);
		bLayoutChanged = true;
	}

	if (IconImage->GetBrush().GetResourceObject() != Marker->IconTexture.Get())
		IconImage->SetBrushFromTexture(Marker->IconTexture, false);

	if (IconImage->GetColorAndOpacity() != Marker->IconTint)
		IconImage->SetColorAndOpacity(Marker->IconTint);

	IconImage->SetVisibility(Marker->IconTexture ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);

	if (bLayoutChanged)
		ForceLayoutPrepass();
}

void UWorldMapMarkerWidget::SetNameOnLeft(bool bLeft)
{
	if (!MarkerContent)
		return;

	const EFlowDirectionPreference Direction = bLeft ? EFlowDirectionPreference::RightToLeft : EFlowDirectionPreference::LeftToRight;
	if (MarkerContent->GetFlowDirectionPreference() != Direction)
		MarkerContent->SetFlowDirectionPreference(Direction);
}

bool UWorldMapMarkerWidget::GetMarkerLayout(bool bNameOnLeft, FVector2D& OutSize, FVector2D& OutIconCenter) const
{
	OutSize = FVector2D::ZeroVector;
	OutIconCenter = FVector2D::ZeroVector;
	if (!HasValidMarkerLayout() || IconAnchor->GetVisibility() == ESlateVisibility::Collapsed)
		return false;

	OutSize = GetDesiredSize();
	const FMargin WidgetPadding = GetPadding();
	const FVector2D ContentSize(OutSize.X - WidgetPadding.Left - WidgetPadding.Right, OutSize.Y - WidgetPadding.Top - WidgetPadding.Bottom);
	const FVector2D IconSize = IconAnchor->GetDesiredSize();
	if (OutSize.X <= 0.0f || OutSize.Y <= 0.0f || IconSize.X <= 0.0f || IconSize.Y <= 0.0f)
		return false;

	float AccumulatedWidth = 0.0f;
	for (int32 ChildIndex = 0; ChildIndex < MarkerContent->GetChildrenCount(); ++ChildIndex)
	{
		const UWidget* Child = MarkerContent->GetChildAt(ChildIndex);
		if (Child->GetVisibility() == ESlateVisibility::Collapsed)
			continue;

		const UHorizontalBoxSlot* ChildSlot = CastChecked<UHorizontalBoxSlot>(Child->Slot);
		const FMargin SlotPadding = ChildSlot->GetPadding();
		if (Child == IconAnchor)
		{
			OutIconCenter.X = AccumulatedWidth + SlotPadding.Left + IconSize.X * 0.5f;
			if (bNameOnLeft)
				OutIconCenter.X = ContentSize.X - OutIconCenter.X;

			OutIconCenter.X += WidgetPadding.Left;

			if (ChildSlot->GetVerticalAlignment() == VAlign_Top)
				OutIconCenter.Y = SlotPadding.Top + IconSize.Y * 0.5f;
			else if (ChildSlot->GetVerticalAlignment() == VAlign_Bottom)
				OutIconCenter.Y = ContentSize.Y - SlotPadding.Bottom - IconSize.Y * 0.5f;
			else
				OutIconCenter.Y = SlotPadding.Top + (ContentSize.Y - SlotPadding.Top - SlotPadding.Bottom) * 0.5f;

			OutIconCenter.Y += WidgetPadding.Top;

			return true;
		}

		AccumulatedWidth += Child->GetDesiredSize().X + SlotPadding.Left + SlotPadding.Right;
	}

	return false;
}
