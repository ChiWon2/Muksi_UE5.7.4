// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"DialogueOption.h"
#include"DialogueMeta.h"
#include"DialogueEvent.h"
#include"../ConditionHandle/CondTree/CondNode.h"
#include"GameplayTagContainer.h"
#include "DialogueRow.generated.h"


USTRUCT(BlueprintType)
struct FDialogueRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Meta", meta = (ShowOnlyInnerProperties))
	FDialogueMeta meta;

	UPROPERTY(EditAnywhere, Category = "Content", meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TSoftObjectPtr<UTexture2D> DialogueImage;

	UPROPERTY(EditAnywhere, Category = "Options", meta = (TitleProperty = "OptionText"))
	TArray<FDialogueOption> Options;

	UPROPERTY(EditAnywhere, Category = "Condition", meta = (AdvancedDisplay))
	FCondNode PopUpConditions;

	UPROPERTY(EditAnywhere, Category = "Events", meta = (AdvancedDisplay))
	TArray<FDialogueEvent> OnEnterEvents;
};