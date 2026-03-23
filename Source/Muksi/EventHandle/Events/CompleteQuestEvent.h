// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "CompleteQuestEvent.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UCompleteQuestEvent : public UBaseEvent
{
	GENERATED_BODY()
public:
	virtual void Execute(const FEventParameter& Param) override;
};
