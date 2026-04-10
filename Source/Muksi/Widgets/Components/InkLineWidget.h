// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InkLineWidget.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;
class USizeBox;

/**
 * 
 */
UCLASS()
class MUKSI_API UInkLineWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetInkText(const FText& InText);
	
	UFUNCTION(BlueprintCallable)
	void RefreshInkWidth();
	
	UFUNCTION(BlueprintCallable)
	void PlayInkLine();
	
	UFUNCTION(BlueprintCallable)
	void ReverseInkLine();
	
	UFUNCTION(BlueprintCallable)
	void ResetInkLine();
	
	UFUNCTION(BlueprintCallable)
	void SetInkProgress(float InProgress);
	
	UPROPERTY(EditAnywhere, Category = "Text")
	FText InkText;
	
protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct()override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface
	
	// ****** BindWidget ******
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> LabelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> InkSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> InkImage;

	// ****** BindWidget ******
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> InkMID;
	
	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ink")
	float InkExtraWidth = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ink")
	float InkHeight = 128.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ink")
	float InkPlaySpeed = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ink")
	bool bAutoReverseOnUnhover = true;

	float CurrentProgress = 0.f;
	bool bPlayForward = false;
	bool bAnimateInk = false;
};
