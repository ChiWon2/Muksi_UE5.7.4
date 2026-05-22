// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestReward.generated.h"

USTRUCT(BlueprintType)
struct FQuestReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int XPReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrencyRewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int> ItemRewards;

	FQuestReward()
		: XPReward(0)
		, CurrencyRewards(0)
	{
	}
};