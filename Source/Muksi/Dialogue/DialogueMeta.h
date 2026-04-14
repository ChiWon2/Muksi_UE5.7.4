// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"DialogueMeta.generated.h"


UENUM(BlueprintType)
enum class EDialogueTriggerType : uint8
{
	Single,
	Reusable,
	ForTown
};

USTRUCT(BlueprintType)
struct FDialogueMeta
{
	GENERATED_BODY()
public: 
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsEntry = false;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsEntry", EditConditionHides))
	EDialogueTriggerType TriggerType = EDialogueTriggerType::Single;

};