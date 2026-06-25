// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Public/Widgets/Widget_ActivatableBase.h"
#include"../QuestDetailRow.h"
#include"../QuestKey.h"
#include "QuestGiverWidget.generated.h"

class UTextBlock;
class UObjectivesWidget;
class UButton;
class UQuestObjectiveEntryWidget;
class UQuestInstance_Base;
class URewardsWidget;
/**
 * 
 */
UCLASS()
class MUKSI_API UQuestGiverWidget : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_QuestName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_QuestDescription;

	UPROPERTY(meta = (BindWidget))
	UObjectivesWidget* ObjectivesWidget;

	UPROPERTY(meta = (BindWidget))
	URewardsWidget* RewardsWidget;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Accept;
	UPROPERTY(meta = (BindWidget))
	UButton* BTN_Decline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FQuestDetailRow Details;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FQuestKey QuestKey;



public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

public:
	void InitWidget(const FQuestKey& InQuestKey);
	UFUNCTION()
	void OnAcceptButtonClicked();
	UFUNCTION()
	void OnDeclineButtonClicked();
	
};
