// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueEvent.h"
#include"../ConditionHandle/CondTree/CondNode.h"
#include"DialogueOption.generated.h"

USTRUCT(BlueprintType)
struct FDialogueOption
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (MultiLine = true))
	FText OptionText;

	UPROPERTY(EditAnywhere)
	FName NextDialogueID;

	UPROPERTY(EditAnywhere, meta = (AdvancedDisplay))
	FCondNode SelectConditions;

	UPROPERTY(EditAnywhere, Category = "Events", meta = (BaseStruct = "/Script/Muksi.Event_Base"))
	TArray<FInstancedStruct> OnSelectEvents;
};
