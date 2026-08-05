// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "BattleCharacterBase.generated.h"

class UCharacterCameraComponent;
class ABattleManager;
class AExchangeCharacterBase;
class UBoxComponent;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;
class UMuksiBattleMovementComponent;
class UMuksiStatusEffectComponent;
class UCharacterPassiveComponent;
class UBattleStatComponent;

USTRUCT(BlueprintType)
struct MUKSI_API FCharacterData
{
	GENERATED_BODY()
	
	void Init_DataAsset(UMuksiCharacterDataAsset* InCharacterData)
	{
		CharacterAsset = InCharacterData;
		MaxHP = InCharacterData->MaxHP;
		AttackValue = InCharacterData->AttackValue;
		CharacterSpeed = InCharacterData->CharacterSpeed;
		DefenseValue = InCharacterData->DefenseValue;
		
		AllBattleDeck = InCharacterData->CharacterDeck;
		BattleDeck = AllBattleDeck;
		CharacterPassives = InCharacterData->CharacterPassiveClass;
	}
	
	bool IsValid() const
	{
		return CharacterAsset != nullptr;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	FHexOffsetCoord CurrentPosition = FHexOffsetCoord();
	
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCharacterDataAsset> CharacterAsset = nullptr;

	UPROPERTY(Transient)
	int32 MaxHP = 0;
	
	UPROPERTY(Transient)
	float AttackValue = 0.f;
	
	UPROPERTY(Transient)
	float DefenseValue = 0.f;

	UPROPERTY(Transient)
	float CharacterSpeed = 1.0f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> BattleDeck;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> AllBattleDeck;
	
	UPROPERTY(Transient)
	TArray<TSubclassOf<UCharacterPassive>> CharacterPassives;
};


UCLASS()
class MUKSI_API ABattleCharacterBase : public AActor, public ISelectableCharacterInterface
{
	GENERATED_BODY()

public:
	ABattleCharacterBase();

public:
	int32 GetCurrentHP() const;
	void SetCurrentHP(int32 NewHP);
	
	FVector2D GetCurrentSelectCardTime()const;
	
	float GetCharacterSpeed()const;

	virtual void SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager);
	
	UMuksiCharacterDataAsset* GetCharacterData()const{return CharacterData.CharacterAsset;};
	
	FHexOffsetCoord GetCharacterCoord()const{return CharacterData.CurrentPosition;};
	void SetCharacterPosition(FHexOffsetCoord NewPosition){CharacterData.CurrentPosition = NewPosition;};
	
	TArray<UMuksiBattleCardDataAsset*> GetCurrentBattleDeck()const{return CharacterData.BattleDeck;};
	void RemoveBattleCard(UMuksiBattleCardDataAsset* BattleCardData);
	
	int32 GetCurrentBattleCardCount()const;
	
	TArray<UMuksiBattleCardDataAsset*> GetAllBattleDeck()const{return CharacterData.AllBattleDeck;};
	void InitBattleDeck(){ CharacterData.BattleDeck = CharacterData.AllBattleDeck; }
	bool RefillBattleDeckIfEmpty()
	{
		if (!CharacterData.BattleDeck.IsEmpty())
		{
			return false;
		}

		CharacterData.BattleDeck = CharacterData.AllBattleDeck;
		return !CharacterData.BattleDeck.IsEmpty();
	}
	
	TArray<TObjectPtr<UCharacterPassive>> GetCharacterPassives();

	UCharacterPassiveComponent* GetPassiveComponent() const
	{
		return PassiveComponent;
	}
	
public:
	UFUNCTION(
		BlueprintPure,
		Category = "Battle Character|Camera"
	)
	FTransform GetCameraFocusTransform() const;
	
	
	
protected:
	void InitializeBattleStats();
	void CopyBattleStateFrom(const ABattleCharacterBase& SourceCharacter);

	FCharacterData CharacterData;
	
public:
	
	UFUNCTION(BlueprintPure, Category = "BattleCharacter|Mesh")
	USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UFUNCTION(BlueprintPure, Category = "BattleCharacter|Movement")
	UMuksiBattleMovementComponent* GetBattleMovementComponent() const { return BattleMovementComponent; }

	UFUNCTION(BlueprintPure, Category = "BattleCharacter|StatusEffect")
	UMuksiStatusEffectComponent* GetStatusEffectComponent() const { return StatusEffectComponent; }
	
	UFUNCTION(BlueprintPure, Category = "BattleCharacter|Stat")
	UBattleStatComponent* GetBattleStatComponent() const { return BattleStatComponent; }
	
	UFUNCTION(BlueprintPure, Category = "BattleCharacter|CameraFocus")
	UCharacterCameraComponent* GetClickCameraFocusComponent() const{ return ClickCameraFocusComponent;}
	
	UFUNCTION(BlueprintPure, Category = "BattleCharacter|CameraFocus")
	UCharacterCameraComponent* GetAttackCameraComponent()const{ return AttackCameraFocusComponent;}



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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleStatComponent> BattleStatComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterCameraComponent> ClickCameraFocusComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterCameraComponent> AttackCameraFocusComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Passive", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterPassiveComponent> PassiveComponent;

};
