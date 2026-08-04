// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_StorySelectScreen.generated.h"

class UMuksiCommonButtonBase;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_StorySelectScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeDestruct() override;

private:
	void CheckSaveGame();
	void HandleNewStoryClicked();
	void ShowNewStoryConfirmScreen();
	void StartNewStory();
	void BindButton();
	void UnBindButton();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMuksiCommonButtonBase>
		Button_NewStory = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMuksiCommonButtonBase>
		Button_Continue = nullptr;


	UPROPERTY(
		EditDefaultsOnly,
		Category = "Story Select|UI"
	)
	TSoftClassPtr<UWidget_ActivatableBase>
		ConfirmScreenClass;


	UPROPERTY(
		EditDefaultsOnly,
		Category = "Story Select|UI"
	)
	FGameplayTag ConfirmWidgetStackTag;

	UPROPERTY(
		EditDefaultsOnly,
		Category = "Story Select|Level"
	)
	FName NewStoryLevelName =
		TEXT("StoryLevel01");
};
