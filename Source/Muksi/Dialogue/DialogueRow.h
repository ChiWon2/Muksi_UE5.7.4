// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"DialogueOption.h"
#include"DialogueMeta.h"
#include"DialogueEvent.h"
#include"GameplayTagContainer.h"
#include"../ConditionHandle/GameCondition.h"

#include "DialogueRow.generated.h"



USTRUCT(BlueprintType)
struct FDialogueRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FDialogueMeta meta;

	UPROPERTY(EditAnywhere)
	TArray<FGameCondition> PopUpConditions;

	UPROPERTY(EditAnywhere)
	TArray<FDialogueEvent> OnEnterEvents;

	UPROPERTY(EditAnywhere)
	FText Text;
	
	UPROPERTY(EditAnywhere)
	TArray<FDialogueOption> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> DialogueImage;
};
