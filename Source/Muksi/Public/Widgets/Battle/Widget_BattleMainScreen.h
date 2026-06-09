// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Widget_BattleMainScreen.generated.h"

class UWidget_CharacterData;
class UHandWidget;
class UInkLineWidget;
class UCharacterDataBase;
class UCharacterData_Enemy;

class UButton;

class ABattleManager;

UENUM(BlueprintType)
enum class EBattleUIPhase : uint8
{	
	None,
	
};

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleMainScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle")
	void BP_OnSelectableCharacterClicked();
	

	
protected:
	//~Begin UCommonActivatableWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnActivated() override;
	//~End UCommonActivatableWidget Interface
	
	
	
	//***** Bound Widgets ****
	UPROPERTY(meta = (BindWidget))
	UHandWidget* HandWidget;
	
	

	//***** Bound Widgets ****
	
private:
	UPROPERTY()
	TObjectPtr<ABattleManager> CachedBattleManager;

	void BindBattleManagerEvents();
	void UnbindBattleManagerEvents();

	UFUNCTION()
	void HandleBattlePhaseChanged(EBattlePhase NewPhase);
	
	UPROPERTY()
	int32 CurrentExchange = 0;
	int32 CurrentAttack = 0;
	
	
public:
	//Handle HandWidget UI
	UFUNCTION(BlueprintCallable)
	void HandleExchangedStarted_(int32 ExchangeNumber);
	
	UFUNCTION(BlueprintCallable)
	void HandleRoundStarted();
	
	UFUNCTION(BlueprintCallable)
	void HandleExchangedStarted();
	
	UFUNCTION(BlueprintCallable)
	void HandleAttackStarted();
	
	UFUNCTION(BlueprintCallable)
	void HandleAttackCoundData(int32 AttackNumber);
	
	UFUNCTION(BlueprintCallable)
	void HandleTurnEnd();
	
	UFUNCTION(BlueprintCallable)
	void FinishCurrentExchange();
	
	UFUNCTION(BlueprintCallable)
	void BindHandWidgetEvents();
	
	UFUNCTION(BlueprintCallable)
	void UnbindHandWidgetEvents();
	
	bool CanRequestEndExchange();
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> EquipBattleCardArray;

protected:

	UPROPERTY()
	TObjectPtr<UCharacterDataBase> PlayerBattleCharacter;
	UPROPERTY()
	TObjectPtr<UCharacterData_Enemy> EnemyBattleCharacter;
	
	UPROPERTY()
	TObjectPtr<UMuksiCharacterDataAsset> PlayerDataAsset = nullptr;
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> EnemySelectBattleCard;

	//Send Info to BattleManager
public:
	UFUNCTION(BlueprintCallable)
	void HandleBattleReady();
	
	
	
	
	UFUNCTION(BlueprintCallable)
	void HandleBattleStarted();
};
