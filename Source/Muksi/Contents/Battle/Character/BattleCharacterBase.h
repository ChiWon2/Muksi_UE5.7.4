// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "BattleCharacterBase.generated.h"

class AExchangeCharacterBase;
enum class ECardAnimType : uint8;
class UMuksiBattleCardDataAsset;
class UMuksiCharacterDataAsset;
class UCharacterDataBase;
class UStaticMeshComponent;
class UMuksiStatusEffectComponent;
class UMuksiBattleAnimationComponent;
class UMuksiBattleMovementComponent;

class USkeletalMesh;
class UAnimInstance;
class UAnimMontage;
class UBoxComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiStatusEffectComponent> StatusEffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiBattleAnimationComponent> BattleAnimationComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	TObjectPtr<UMuksiBattleMovementComponent> BattleMovementComponent;


	UFUNCTION()
	void HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	
	
	UPROPERTY()
	FIntPoint CharacterPosition = FIntPoint(0,0);
	//FCharacterDisplayData CharacterDisplayData;
	
public:
	UFUNCTION(BlueprintPure, Category = "BattleCharacter|Movement")
	UMuksiBattleMovementComponent* GetBattleMovementComponent() const
	{
		return BattleMovementComponent;
	}
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual void SetCharacterData(UCharacterDataBase* InCharacterData);
	
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	UCharacterDataBase* GetCharacterData() const {return CharacterData;};
	
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual void SetExchangeCharacter(AExchangeCharacterBase* ExchangeCharacter){ExchangeCharacterBase = ExchangeCharacter;};
	
	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	virtual AExchangeCharacterBase* GetExchangeCharacter(){return ExchangeCharacterBase;}
	
	UFUNCTION()
	FIntPoint GetCharacterPosition(){return CharacterPosition;};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UFUNCTION(BlueprintCallable, Category = "BattleCharacter")
	UMuksiStatusEffectComponent* GetStatusEffectComponent() const{return StatusEffectComponent;}
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")

	TSubclassOf<UAnimInstance> AnimInstanceClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DefaultAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClickFunction")
	TObjectPtr<UBoxComponent> ClickCollision;

	//공격 애니메이션 관련 함수
public:
	void PlayAttackAnim(UMuksiBattleCardDataAsset* BattleCardData);

protected:
	TObjectPtr<UAnimMontage> FindAnimations(ECardAnimType CardAnim)const;
	
	TObjectPtr<UAnimMontage> AnimMontage = nullptr;
	
	TObjectPtr<AExchangeCharacterBase> ExchangeCharacterBase = nullptr;
public:
	//virtual FCharacterDisplayData GetCharacterDisplayData() const override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	
	
	
	
	

};
