// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_Battle.generated.h"

class UDataTable;
class UCharacterDataBase;
class UMuksiCharacterDataAsset;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class MUKSI_API UPlayerMode_Battle : public UPlayerModeBase
{
	GENERATED_BODY()
public:
	//~ Begin UPlayerModeBase Interface
	virtual void EnterMode(AMuksiPlayerController* PlayerController)override;
	virtual void ExitMode()override;
	virtual int32 GetInputMappingPriority() const override { return 10; }
	
	virtual void HandleLeftClick(const FInputActionValue& Value) override;
	//~ End UPlayerModeBase Interface
	
	//TEST
public:
	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	void InitializeBattleTestData();

	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	UCharacterDataBase* GetPlayerCharacterData() const { return PlayerCharacterData; }

	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	UCharacterDataBase* GetEnemyCharacterData() const { return EnemyCharacterData; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UDataTable> BattleCardDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UMuksiCharacterDataAsset> TestPlayerCharacterDataAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UMuksiCharacterDataAsset> TestEnemyCharacterDataAsset = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterDataBase> PlayerCharacterData = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterDataBase> EnemyCharacterData = nullptr;
	
	
	
};
