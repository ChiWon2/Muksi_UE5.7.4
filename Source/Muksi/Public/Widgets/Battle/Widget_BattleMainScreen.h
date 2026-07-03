// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Widget_BattleMainScreen.generated.h"

class UWidget_CharacterData;
class UHandWidget;
class UInkLineWidget;


class ABattleCharacter_Player;
class ABattleCharacter_Enemy;

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

	
	UPROPERTY()
	int32 CurrentExchange = 0;
	int32 CurrentAttack = 0;
	
	
public:

	
	UFUNCTION(BlueprintCallable)
	void BindHandWidgetEvents();
	
	UFUNCTION(BlueprintCallable)
	void UnbindHandWidgetEvents();
	
	bool CanRequestEndExchange();
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> EquipBattleCardArray;

protected:

	UPROPERTY()
	TObjectPtr<ABattleCharacter_Player> PlayerBattleCharacter;
	UPROPERTY()
	TObjectPtr<ABattleCharacter_Enemy> EnemyBattleCharacter;
	
	UPROPERTY()
	TObjectPtr<UMuksiCharacterDataAsset> PlayerDataAsset = nullptr;
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> EnemySelectBattleCard;

	//---------------------------중앙 텍스트 블록 관련 함수---------------------------------------------------------------
public:
	void DisplayMainTextBlock(FText Text, float Time);
	
protected:
	void DisplayExchangeAlarm();
	void DisplayExchangeCountAlarm(int32 ExchangeCount, bool bStart);
	UPROPERTY(EditAnywhere)
	FString DisplayExchangeText = "합 시작!";
	//------------------------------------------------------------------------------------------------------------------
	//BattleManager에 정보를 보내는 함수
	
	
	void ChangePhase(EBattlePhase NewPhase);
	
	//====================================Ready<준비>===================================================================
public:
	void ReadyStart();
	void ReadyEnd();
	
	void SetBattleManager();
	
	/*UFUNCTION(BlueprintCallable)
	void HandleBattleReady();
	
	UFUNCTION(BlueprintCallable)
	void HandleBattleStarted();*/
	
	//====================================Battle<전투>==================================================================
public:
	void BattleStart();
	
	void BattleEnd();
	
	void HandleBattleStartFinish();
	
	void HandleBattleEndFinish();
public:
	UPROPERTY(EditAnywhere)
	FString BattleStartText = "전 투 시 작!";
	int32 HandleBattleFinishCount = 0;	
	
	//=====================================Round<국>====================================================================
public:
	//국 시작
	void RoundStart();
	
	void HandleRoundStartFinish();
	
	//국 종료
	void RoundEnd();
	
	void HandleRoundEndFinish();
	
public:
	UPROPERTY(EditAnywhere)
	FString RoundStartText = "국 시 작!";
	
	UPROPERTY()
	int32 HandleRoundStartFinishCount = 0;	
	
	UPROPERTY()
	int32 RoundCound = 0;
	
	//=================================Exchange<합>=====================================================================
	//합 시작	Round 시작
	
	//1합 시작
	//1합 종료
	
	//2합 시작
	//2합 종료
	
	//3합 시작
	//3합 종료
	
	//합 종료
	
public:
	void ExchangeStart();
	
	void HandleExchangeStartFinish();
	
	void Exchange1Start();
	void Exchange1End();
	
	void HandleExchange1EndFinish();
	
	void Exchange2Start();
	void Exchange2End();
	
	void HandleExchange2EndFinish();
	
	void Exchange3Start();
	void Exchange3End();
	
	void HandleExchange3EndFinish();
	
	void ExchangeEnd();
	
	void HandleExchangeEndFinish();
	
protected:
	void HandleExchangeSlot(int32 Index, bool bActive);
	
public:
	int32 HandleExchangeCount = 0;
	
public:
	//합 도중 카드 선택 확정 버튼
	UFUNCTION(BlueprintCallable)
	void HandleCardSelect();
	
	bool CanRequestSelectCard();
	void SelectCardDataSend(int32 InIndex);
	
	void FinishSelectCard();
	//------------------------------------------------------------------------------------------------------------------
	
	//=================================Attack<공격>=====================================================================
	
public:
	void AttackStart();
	
	void PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter, ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset);
	
	void AttackEnd();
};
