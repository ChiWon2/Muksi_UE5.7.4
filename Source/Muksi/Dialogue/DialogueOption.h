// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueEvent.h"
#include"../ConditionHandle/GameCondition.h"
#include"DialogueOption.generated.h"

USTRUCT(BlueprintType)
struct FDialogueOption
{
	GENERATED_BODY()
public:
    // Text of UI
    UPROPERTY(EditAnywhere)
    FText OptionText;

    // If you choose this Option, Move to NextDialogueID
    UPROPERTY(EditAnywhere)
    FName NextDialogueID;

    UPROPERTY(EditAnywhere)
    TArray<FDialogueEvent> OnSelectEvents;

    //Conditions required to select this option
    UPROPERTY(EditAnywhere)
    TArray<FGameCondition> OptionConditions;

};
