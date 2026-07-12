// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	float GetCharacterSpeed() const;

	FIntPoint GetCharacterPosition() const { return CharacterPosition; }
	void SetCharacterPosition(const FIntPoint& NewPosition) { CharacterPosition = NewPosition; }

	TArray<UMuksiBattleCardDataAsset*> GetCurrentBattleDeck() const { return CharacterData ? CharacterData->GetCharacterDeck() : TArray<UMuksiBattleCardDataAsset*>(); }
	void RemoveBattleCard(UMuksiBattleCardDataAsset* BattleCardData) const { if (CharacterData) CharacterData->RemoveCard(BattleCardData); }

	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual void SetCharacterData(UCharacterDataBase* InCharacterData);

	UFUNCTION(BlueprintPure, Category = "BattleCharacter")
	UCharacterDataBase* GetCharacterData() const { return CharacterData; }

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiStatusEffectComponent> StatusEffectComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiBattleAnimationComponent> BattleAnimationComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiBattleMovementComponent> BattleMovementComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UMuksiCharacterDataAsset> CharacterDataAsset = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterDataBase> CharacterData = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AExchangeCharacterBase> ExchangeCharacterBase = nullptr;

	UPROPERTY(Transient)
	FIntPoint CharacterPosition = FIntPoint::ZeroValue;
};
