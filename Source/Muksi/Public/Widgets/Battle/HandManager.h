// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HandManager.generated.h"

class UCardObject;
/**
 * 
 */
UCLASS(BlueprintType)
class MUKSI_API UHandManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize();
	void DrawInitialHands(int32 InDrawCount = 5);
	
	const TArray<TObjectPtr<UCardObject>> & GetHandCards() const {return HandCards;}
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UCardObject>> HandCards;
};
