// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveActivePopup.generated.h"

class UImage;
class UTextBlock;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class MUKSI_API UPassiveActivePopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetData(UTexture2D* CharacterImage, FText PassiveName);
	void StartRemoveTimer(float LifeTime = 3.0f);
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* Image_CharacterImage;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlock_PassiveName;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Show;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Hide;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
	
	UFUNCTION()
	void RemovePopup();
	
private:
	FTimerHandle RemoveTimerHandle;
	
	bool bRemoving = false;
};
