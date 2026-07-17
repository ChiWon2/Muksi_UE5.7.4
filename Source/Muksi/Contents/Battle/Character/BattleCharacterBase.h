// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "BattleCharacterBase.generated.h"

class AExchangeCharacterBase;
class UBoxComponent;
class UCharacterDataBase;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;
class UMuksiBattleMovementComponent;
class UMuksiCharacterDataAsset;
class UMuksiStatusEffectComponent;

USTRUCT(BlueprintType)
struct MUKSI_API FCharacterData
{
	GENERATED_BODY()
	
	void Init(UMuksiCharacterDataAsset* InCharacterData)
	{
		CharacterAsset = InCharacterData;
		MaxHP = InCharacterData->MaxHP;
		CurrentHP = InCharacterData->MaxHP;
		CharacterSpeed = InCharacterData->CharacterSpeed;
		AllBattleDeck = InCharacterData->CharacterDeck;
	}
	
	bool IsValid() const
	{
		return CharacterAsset != nullptr;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FIntPoint CurrentPosition = FIntPoint::ZeroValue;
	
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCharacterDataAsset> CharacterAsset = nullptr;

	UPROPERTY(Transient)
	int32 CurrentHP = 0;

	UPROPERTY(Transient)
	int32 MaxHP = 0;

	UPROPERTY(Transient)
	float CharacterSpeed = 1.0f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> BattleDeck;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> AllBattleDeck;
};


UCLASS()
class MUKSI_API ABattleCharacterBase : public AActor, public ISelectableCharacterInterface
{
	GENERATED_BODY()

public:
	ABattleCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	int32 GetCurrentHP() const;
	void SetCurrentHP(int32 NewHP);
	
	FVector2D GetCurrentSelectCardTime()const;
	
	float GetCharacterSpeed()const;

	virtual void SetCharacterData(UMuksiCharacterDataAsset* InCharacterData);
	
	FIntPoint GetCharacterPosition()const{return CharacterData.CurrentPosition;};
	void SetCharacterPosition(FIntPoint NewPosition){CharacterData.CurrentPosition = NewPosition;};
	
	TArray<UMuksiBattleCardDataAsset*> GetCurrentBattleDeck()const{return CharacterData.BattleDeck;};
	void RemoveBattleCard(UMuksiBattleCardDataAsset* BattleCardData);
	
	int32 GetCurrentBattleCardCount()const;
	
	TArray<UMuksiBattleCardDataAsset*> GetAllBattleDeck()const{return CharacterData.AllBattleDeck;};
	void InitBattleDeck(){CharacterData.BattleDeck = CharacterData.AllBattleDeck;};
	
protected:
	FCharacterData CharacterData;
	
public:
	

	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual void SetExchangeCharacter(AExchangeCharacterBase* InExchangeCharacter) { ExchangeCharacterBase = InExchangeCharacter; }

	UFUNCTION(BlueprintPure, Category = "BattleCharacter")
	AExchangeCharacterBase* GetExchangeCharacter() const { return ExchangeCharacterBase; }

	UFUNCTION(BlueprintPure, Category = "BattleCharacter|Movement")
	UMuksiBattleMovementComponent* GetBattleMovementComponent() const { return BattleMovementComponent; }

	UFUNCTION(BlueprintPure, Category = "BattleCharacter|StatusEffect")
	UMuksiStatusEffectComponent* GetStatusEffectComponent() const { return StatusEffectComponent; }

	virtual void OnSelected() override;
	virtual void OnDeselected() override;

protected:
	UFUNCTION()
	void HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ClickCollision = nullptr;

public:
	//Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiStatusEffectComponent> StatusEffectComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiBattleAnimationComponent> BattleAnimationComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiBattleMovementComponent> BattleMovementComponent = nullptr;
	
	
	UPROPERTY(Transient)
	TObjectPtr<AExchangeCharacterBase> ExchangeCharacterBase = nullptr;

};
