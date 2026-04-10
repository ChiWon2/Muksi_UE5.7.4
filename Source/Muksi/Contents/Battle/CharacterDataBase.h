// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Muksi/Contents/Battle/Data/MMuksiBattleCardTableRow.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataBase.generated.h"

/**
 * 
 */


class UMuksiCharacterDataAsset;
class UMuksiBattleCardDataAsset;


UCLASS(BlueprintType, Blueprintable)
class MUKSI_API UCharacterDataBase : public UObject
{
	GENERATED_BODY()
	
public:
	void InitializeFromDataAsset(UMuksiCharacterDataAsset* InCharacterAsset, UDataTable* InCardDataTable);

	UFUNCTION(BlueprintCallable, Category = "Character")
	UMuksiCharacterDataAsset* GetSourceCharacterAsset() const { return SourceCharacterAsset; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	int32 GetCurrentHP() const { return CurrentHP; }

	const TArray<FName>& GetDeckCardRowNames() const { return DeckCardRowNames; }

	const FMMuksiBattleCardTableRow* FindCardRow(FName InRowName) const;
	
	UDataTable* GetCardDataTable() const { return CardDataTable; }



protected:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCharacterDataAsset> SourceCharacterAsset = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CardDataTable = nullptr;

	UPROPERTY(Transient)
	int32 CurrentHP = 0;

	UPROPERTY(Transient)
	TArray<FName> DeckCardRowNames;
};
