// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DialogueOption.h"
#include "DialogueOptionWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionButtonClicked, int32, OptionIndex);

UCLASS()
class MUKSI_API UDialogueOptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintCallable)
	FOnOptionButtonClicked OnOptionButtonClicked;

	void NativeConstruct() override;
	void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_OptionText;
	UPROPERTY(meta = (BindWidget))
	UButton* BTN_OptionButton;

	UFUNCTION(BlueprintCallable)
	void InitWidget(int32 InOptionIndex, const FText& InText);
protected:
	UFUNCTION()
	void HandleOptionButtonClicked();

private:
	int32 OptionIndex;

};
