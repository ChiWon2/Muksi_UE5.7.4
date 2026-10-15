#include "Widgets/Battle/FloatingText/Widget_BattleFloatingText.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWidget_BattleFloatingText::SetFloatingText(float HPChange, const FBattleFloatingTextData& Data)
{
	if (!ValueText || !FloatingTextRoot)
		return;

	const int32 Value = FMath::RoundToInt(FMath::Abs(HPChange));
	ValueText->SetText(FText::FromString(FString::Printf(TEXT("%s%d"), HPChange > 0.0f ? TEXT("+") : TEXT("-"), Value)));
	ValueText->SetColorAndOpacity(FSlateColor(Data.Style.Color));
	FSlateFontInfo ValueFont = ValueText->GetFont();
	ValueFont.Size = FMath::Max(1, Data.Style.FontSize);
	ValueText->SetFont(ValueFont);

	if (DescriptionText)
	{
		DescriptionText->SetText(Data.Description);
		DescriptionText->SetVisibility(Data.Description.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
		FSlateFontInfo DescriptionFont = DescriptionText->GetFont();
		DescriptionFont.Size = FMath::Max(1, Data.Style.DescriptionFontSize);
		DescriptionText->SetFont(DescriptionFont);
	}

	if (StatusIcon)
	{
		StatusIcon->SetBrushFromTexture(Data.Icon);
		StatusIcon->SetVisibility(Data.Icon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		StatusIcon->SetColorAndOpacity(Data.Style.Color);
	}

	AnimationSettings = Data.AnimationStyle;
	ElapsedTime = 0.0f;
	bFloatingTextActive = true;
	FloatingTextRoot->SetRenderTranslation(FVector2D::ZeroVector);
	FloatingTextRoot->SetRenderOpacity(1.0f);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UWidget_BattleFloatingText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bFloatingTextActive || !FloatingTextRoot)
		return;

	ElapsedTime += InDeltaTime;

	const float Duration = FMath::Max(0.01f, AnimationSettings.Duration);
	const float Progress = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
	FloatingTextRoot->SetRenderTranslation(FVector2D(0.0f, -AnimationSettings.MoveDistance * Progress));

	const float FadeStartTime = FMath::Clamp(AnimationSettings.FadeOutStartTime, 0.0f, Duration);
	const float FadeDuration = Duration - FadeStartTime;
	const float Opacity = FadeDuration > KINDA_SMALL_NUMBER ? 1.0f - FMath::Clamp((ElapsedTime - FadeStartTime) / FadeDuration, 0.0f, 1.0f) : (ElapsedTime < Duration ? 1.0f : 0.0f);
	FloatingTextRoot->SetRenderOpacity(Opacity);

	if (ElapsedTime >= Duration)
	{
		bFloatingTextActive = false;
		RemoveFromParent();
	}
}
