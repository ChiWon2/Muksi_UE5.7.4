#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CharacterInfoPanel.generated.h"

class UButton;
class UScrollBox;
class UStatComponent;
class UTextBlock;

UCLASS()
class MUKSI_API UWidget_CharacterInfoPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void RefreshStats();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SummaryStatText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> DetailInfoButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> DetailStatScrollBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailStatText;

private:
	UFUNCTION()
	void HandleStatChanged();

	UFUNCTION()
	void HandleDetailInfoClicked();

	UStatComponent* GetPlayerStatComponent() const;
	void BindStatComponent(UStatComponent* NewStatComponent);
	void SetTextIfValid(UTextBlock* TextBlock, const FText& Text) const;

	UPROPERTY(Transient)
	TObjectPtr<UStatComponent> CachedStatComponent = nullptr;

	bool bDetailVisible = false;
};