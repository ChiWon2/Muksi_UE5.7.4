// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "StartBattleEvent.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UStartBattleEvent : public UBaseEvent
{
	GENERATED_BODY()
public:
	void Execute(const FEventParameter& Param);

};
