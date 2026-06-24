// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Public/Widgets/Widget_ActivatableBase.h"
#include"../QuestDetailRow.h"
#include"../QuestKey.h"
#include"../QuestReward.h"
#include "QuestRewardWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UButton;
class UQuestObjectiveEntryWidget;
class UQuestInstance_Base;
class URewardsWidget;

/**
 * 
 */
UCLASS()
class MUKSI_API UQuestRewardWidget : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_QuestName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_QuestDescription;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_ClearQuest;
	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Cancel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FQuestDetailRow Details;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FQuestKey QuestKey;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UQuestObjectiveEntryWidget> QuestObjectiveEntryWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UQuestInstance_Base* CurrentQuestInstance;

	UPROPERTY(meta = (BindWidget))
	URewardsWidget* RewardsWidget;

private:
	FQuestReward QuestReward;

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

public:
	void InitWidget(const FQuestKey& InQuestKey);
	UFUNCTION()
	void OnClearQuestButtonClicked();
	UFUNCTION()
	void OnCancelButtonClicked();
private:
	void RefreshObjectives();
	void GiveReward();
};
