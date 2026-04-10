// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiCharacterDataAsset.generated.h"

class UMuksiBattleCardDataAsset;

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FName CharacterID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UTexture2D> CharacterIllustration = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText FactionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (MultiLine = true))
	FText FactionDescription;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Passive")
	TArray<TObjectPtr<UMuksiPassiveDataAsset>> SelfPassives;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Passive")
	TArray<TObjectPtr<UMuksiPassiveDataAsset>> AllyPassives;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Deck")
	TArray<FName> StartingDeckCardRowNames;
};
