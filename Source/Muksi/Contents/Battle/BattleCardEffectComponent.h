// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/MuksiBattleCardEffectData.h"
#include "BattleCardEffectComponent.generated.h"

class UBattleCardPreviewComponent;
class ABattleManager;
class ABattleGridManager;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;
class UCharacterDataBase;
class ABattleGridTile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleCardEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleCardEffectComponent();
	
	void InitializePreviewComponent(
		ABattleManager* InBattleManager,
		ABattleGridManager* InGridManager,
		UBattleCardPreviewComponent* InCardPreviewComponent
	);
	
	void CardEffectUpdate(ABattleCharacterBase* BattleCharacterBase, UMuksiBattleCardDataAsset* InCardData);
	
	void SelectTargetGrid(ABattleGridTile* TargetGrid);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	FMuksiBattleCardAttackTypeData PlayerEffect;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;

	UPROPERTY()
	TObjectPtr<ABattleGridManager> GridManager;

	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> SourceCharacter;

	UPROPERTY()
	TObjectPtr<UMuksiBattleCardDataAsset> CurrentCardData;
	
	UPROPERTY()
	TObjectPtr<UBattleCardPreviewComponent> BattleCardPreviewComponent;

		
};
