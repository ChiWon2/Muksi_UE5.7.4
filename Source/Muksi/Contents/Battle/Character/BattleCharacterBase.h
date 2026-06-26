// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "BattleCharacterBase.generated.h"

class UMuksiBattleCardDataAsset;
class UMuksiCharacterDataAsset;
class UCharacterDataBase;
class UStaticMeshComponent;

UCLASS()
class MUKSI_API ABattleCharacterBase : public AActor, public ISelectableCharacterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
public:
	int32 GetCurrentHP()const;
	void SetCurrentHP(int32 NewHP);
	
	
	FIntPoint GetCharacterPosition()const{return CharacterPosition;};
	void SetCharacterPosition(FIntPoint NewPosition){CharacterPosition = NewPosition;};
	
	TArray<UMuksiBattleCardDataAsset*> GetCurrentBattleDeck()const{return CharacterData->GetCharacterDeck();};
	void RemoveBattleCard(UMuksiBattleCardDataAsset* BattleCardData)const{CharacterData->RemoveCard(BattleCardData);};
	
	UPROPERTY()
	TObjectPtr<UMuksiCharacterDataAsset> CharacterDataAsset = nullptr;
	
protected:
	UPROPERTY()
	TObjectPtr<UCharacterDataBase> CharacterData = nullptr;
	
	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UFUNCTION()
	void HandleClicked(AActor* TouchedActor, FKey ButtonPressed);
	
	
	UPROPERTY()
	FIntPoint CharacterPosition = FIntPoint(0,0);
	//FCharacterDisplayData CharacterDisplayData;
	
public:
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual void SetCharacterData(UCharacterDataBase* InCharacterData);
	
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	UCharacterDataBase* GetCharacterData() const {return CharacterData;};
	
	UFUNCTION()
	FIntPoint GetCharacterPosition(){return CharacterPosition;};

	

public:
	//virtual FCharacterDisplayData GetCharacterDisplayData() const override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	
	
	
	
	

};
