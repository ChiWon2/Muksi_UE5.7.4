// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/Panic/PanicStrategyBase.h"
#include "PanicStrategy_Self.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UPanicStrategy_Self : public UPanicStrategyBase
{
	GENERATED_BODY()
public:
	virtual FPanicStrategyResult SelectTarget_Implementation(const FPanicStrategyContext& Context)override;
};
