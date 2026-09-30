// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EffectDescriptionPopup.generated.h"

class UTextBlock;
class UImage;
class UBorder;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class MUKSI_API UEffectDescriptionPopup : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetEffectData(
		const FText& EffectName,
		const FText& EffectDescription,
		const FSlateBrush& EffectIcon
	);
	
	
	
	

	void ShowByHover(
		FName EffectID,
		const FText& EffectName,
		const FText& Description,
		const FSlateBrush& IconBrush,
		const FVector2D& Position
	);

	void HideByHover();


	

	
protected:
	virtual void NativeConstruct() override;
	void ShowAtPosition(const FVector2D& Position);
	void HidePopup();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EffectIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectDescriptionText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Center;
	
};
