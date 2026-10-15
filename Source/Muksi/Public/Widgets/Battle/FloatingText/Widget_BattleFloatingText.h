#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleFloatingTextTypes.h"
#include "Widget_BattleFloatingText.generated.h"

class UHorizontalBox;
class UTextBlock;
class UImage;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UWidget_BattleFloatingText : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetFloatingText(float HPChange, const FBattleFloatingTextData& Data);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> FloatingTextRoot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> StatusIcon;

private:
	FBattleFloatingTextAnimationStyle AnimationSettings;
	float ElapsedTime = 0.0f;
	bool bFloatingTextActive = false;
};
