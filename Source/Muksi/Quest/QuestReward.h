// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestReward.generated.h"

USTRUCT(BlueprintType)
struct FItemReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FQuestReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int XPReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrencyRewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemReward> ItemRewards;

	FQuestReward()
		: XPReward(0)
		, CurrencyRewards(0)
	{
	}
};