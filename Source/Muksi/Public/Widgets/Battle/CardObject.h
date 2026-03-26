// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CardObject.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UCardObject : public UObject
{
	GENERATED_BODY()
public:
	void InitCard(const FName& InCardID, const FText& InCardName);
	
	FName GetCardID() const {return CardID;}
	FText GetCardName() const {return CardName;}
	
private:
	UPROPERTY()
	FName CardID = NAME_None;
	
	UPROPERTY()
	FText CardName;
};
