// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "AddStatEvent.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UAddStatEvent : public UBaseEvent
{
	GENERATED_BODY()
public:
	void Execute(const FEventParameter& Param) override;
};
