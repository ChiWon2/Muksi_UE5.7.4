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
	void InitializeFromDataAsset(UMuksiCharacterDataAsset* InCharacterAsset);
	

	UFUNCTION(BlueprintCallable, Category = "Character")
	UMuksiCharacterDataAsset* GetSourceCharacterDataAsset() const { return SourceCharacterAsset; }
	
	
	/*UFUNCTION(BlueprintCallable, Category = "Character")
	void SetSourceCharacterDataAsset(UMuksiCharacterDataAsset* InCharacterDataAsset){SourceCharacterAsset = InCharacterDataAsset;}*/

	UFUNCTION(BlueprintCallable, Category = "Character")
	int32 GetCurrentHP() const { return CurrentHP; }

	//const TArray<FName>& GetDeckCardRowNames() const { return DeckCardRowNames; }

	//const FMMuksiBattleCardTableRow* FindCardRow(FName InRowName) const;
	
	//UDataTable* GetCardDataTable() const { return CardDataTable; }


	UFUNCTION(BlueprintCallable, Category = "Character")
	void ApplyDamage(int32 InDamage);

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsDead() const { return CurrentHP <= 0; }
protected:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCharacterDataAsset> SourceCharacterAsset = nullptr;
	

	UPROPERTY(Transient)
	int32 CurrentHP = 0;


	
protected:
	UPROPERTY()
	TObjectPtr<UMuksiCharacterDataAsset> CharacterDataAsset;
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> BattleDeck;
	
public:
	UFUNCTION()
	TArray<UMuksiBattleCardDataAsset*> GetCharacterDeck();
	
	UFUNCTION()
	void SetMuksiCharacterDataAsset(UMuksiCharacterDataAsset* CharacterDataAsset_){CharacterDataAsset = CharacterDataAsset_;};
	UFUNCTION(BlueprintCallable)
	bool RemoveCard(UMuksiBattleCardDataAsset* CardData);
	
	
};
