// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_Test.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_Test : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
};
