// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_Battle.generated.h"

class UWidget_BattleMainScreen;
class UDataTable;
class UCharacterDataBase;
class UMuksiCharacterDataAsset;
class UWidget_ActivatableBase;
class ABattleCharacterBase;

class ABattleGridTile;
class ABattleManager;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class MUKSI_API UPlayerMode_Battle : public UPlayerModeBase
{
	GENERATED_BODY()

public:
	//~ Begin UPlayerModeBase Interface
	virtual void EnterMode(AMuksiPlayerController* PlayerController) override;
	virtual void ExitMode() override;
	virtual void TickPlayerMode() override;
	virtual int32 GetInputMappingPriority() const override { return 10; }

	virtual void HandleLeftClick(const FInputActionValue& Value) override;
	virtual void HandleRPressedKey(const FInputActionValue& Value) override;
	//~ End UPlayerModeBase Interface

	//TEST
	void UpdateHoveredGridTile();

	UPROPERTY()
	TObjectPtr<ABattleGridTile> HoveredGridTile = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	void InitializeBattleTestData();

	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	ABattleCharacterBase* GetPlayerCharacterData() const { return BattleCharacterPlayer; }

	UFUNCTION(BlueprintCallable, Category = "Battle Test")
	ABattleCharacterBase* GetEnemyCharacterData() const { return BattleCharacterEnemy; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UDataTable> BattleCardDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UMuksiCharacterDataAsset> TestPlayerCharacterDataAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Test")
	TObjectPtr<UMuksiCharacterDataAsset> TestEnemyCharacterDataAsset = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> BattleCharacterPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> BattleCharacterEnemy = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY()
	TObjectPtr<UWidget_BattleMainScreen> BattleMainScreen = nullptr;

private:
	void UpdateCardTargeting(const FHitResult& HitResult);
	void PushCharacterDataWidget();

	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Battle UI")
	TSoftClassPtr<UWidget_ActivatableBase> WidgetCharacterDataClass;
};