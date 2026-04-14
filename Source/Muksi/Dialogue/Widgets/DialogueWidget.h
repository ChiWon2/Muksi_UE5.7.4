// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"DialogueOptionWidget.h"
#include "DialogueWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UDialogueSubsystem;
class UTravelTimeSubsystem;
class URichTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;
	void NativeDestruct() override;
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_DialogueID; //ForTest

	UPROPERTY(meta= (BindWidget))
	URichTextBlock* TXT_DialogueText;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Options;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UDialogueOptionWidget> DialogueOptionWidgetClass;
	

	UFUNCTION()
	void OnDialogueTextUpdated(const FText& NewText);

	UFUNCTION()
	void OnDialogueOptionsUpdated(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnDialogueEnded();

	UFUNCTION()
	void HandleOptionButtonClicked(int32 OptionIndex);


	UFUNCTION(BlueprintCallable)
	void InitWidget();
	UFUNCTION(BlueprintCallable)
	void BindToSubsystem();

private:
	void StartTravelTime();
	void StopTravelTime();
private:
	UPROPERTY()
	UDialogueSubsystem* DialogueSubSystem;
	UPROPERTY()
	UTravelTimeSubsystem* TravelTimeSubsystem;
};
