// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Widget_BattleMainScreen.generated.h"

class UCardPreviewPanel;
class UBattleStatusHUDWidget;
class UWidget_CharacterData;
class UInkLineWidget;
class UPassiveActivePopupWidget;
class UMuksiBattleCardDataAsset;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattlePipelineWidget;
class UWidget_BattleControlPanel;
class UWidget_BattleSkillRevealPanel;

class ABattleCharacterBase;
class UButton;

class ABattleManager;
class ABattleTargetingManager;
class ABattleSequenceManager;
class ABattleSimulationManager;

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

	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnActivated() override;
	//~End UCommonActivatableWidget Interface

	//***** Bound Widgets ****
	
	UPROPERTY(meta = (BindWidget))
	UCardPreviewPanel* CardPreviewPanel;

	UPROPERTY(meta = (BindWidget))
	UPassiveActivePopupWidget* ActivePassiveWidget;
	
	UPROPERTY(meta = (BindWidget))
	UBattleStatusHUDWidget* StatusHUDWidget;
	
	UPROPERTY(meta = (BindWidget))
	UBattlePipelineWidget* BattlePipelineWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleControlPanel> BattleControlPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealPanel> BattleSkillRevealPanel;

	//***** Bound Widgets ****
private:
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;

	UPROPERTY()
	TObjectPtr<ABattleTargetingManager> BattleTargetingManager;

	UPROPERTY()
	TObjectPtr<ABattleSequenceManager> BattleSequenceManager;

	UPROPERTY()
	TObjectPtr<ABattleSimulationManager> BattleSimulationManager;

	UPROPERTY(Transient)
	TObjectPtr<UBattlePhaseTask> PhaseUITask;

public:
	UFUNCTION()
	void SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);
	
	UFUNCTION(BlueprintCallable)
	void BindBattlePipelineWidgetEvents();
	
	UFUNCTION(BlueprintCallable)
	void UnbindBattlePipelineWidgetEvents();
	
	


	bool CanRequestEndExchange();

	// 카드 슬롯에서 장착 해제될 때 진행 중인 타겟팅/대기 카드 상태를 함께 정리한다.
	void NotifyPlayerCardUnequipped();
	

	//---------------------------중앙 텍스트 블록 관련 함수---------------------------------------------------------------
public:
	void HandlePipelineUIFinish();

protected:
	//------------------------------------------------------------------------------------------------------------------
	void BindBattleManagerEvents();
	void UnbindBattleManagerEvents();
	void BindBattleSequenceManagerEvents();
	void UnbindBattleSequenceManagerEvents();
	
	void BindBattleSkillReveal();
	void UnbindBattleSkillReveal();
	
	void BindBattleSkillEvent();
	void UnbindBattleSkillEvent();
	
	void BindBattleControlPanelEvents();
	void UnbindBattleControlPanelEvents();
	void HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData);
	void HandleBattleSkillHovered(UMuksiBattleCardDataAsset* SkillData);
	void HandleBattleSkillUnhovered();
	
	void HandlePlayerTargetingCancelled();
	
	void BattlePipelineWidgetSetting(EBattlePhase BattlePhase);

	UFUNCTION()
	void HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	void CompletePhaseUI(EBattlePhase FinishedPhase);

	void HandleDeceiveCardRevealRequested(const FBattleAction& BattleAction);
	
	UPROPERTY()
	int32 HandleUIFinishCount = 0;

	//====================================Ready<준비>===================================================================
public:
	void ReadyStart();
	void ReadyEnd();

	//====================================Battle<전투>==================================================================
public:
	void BattleStart();
	
	void DisplayBattleStartUIFinish();

	void BattleEnd();
	
	void DisplayBattleEndUIFinish();

	void HandleBattleStartFinish();

	void HandleBattleEndFinish();

	//=====================================Round<국>====================================================================
public:
	//국 시작
	void RoundStart();
	
	void DisplayRoundStartUIFinish();


	//국 종료
	void RoundEnd();
	
	
	void DisplayRoundEndUIFinish();

	void HandleRoundEndFinish();
	

	//=================================Exchange<합>=====================================================================
	//합 시작	Round 시작

public:
	void ExchangeStart();
	
	void DisplayExchangeStartUIFinish();
	
	void DisplayExchangeEndUIFinish();

	void StartExchangeSelectCard(int32 ExchangeIndex);
	void FinishExchange(int32 ExchangeIndex);

	void ExchangeEnd();

	void HandleExchangeEndFinish();

protected:
	void HandleSkillSlot(bool bActive);
	void SetBattleCardToHand();
	void SetBattleSkillSetting();
	
	void HandleSkillRevealFinished(int32 ExchangeIndex);
	void TimeOutCardSelect();
	
	bool UpdateCurrentExchangeSkillReveal();
public:
	
	void HandleEnemyCardSelectionReady(UMuksiBattleCardDataAsset* EnemyCard, int32 ExchangeIndex);

public:
	//합 도중 카드 선택 확정 버튼
	
	//------------------------------------------------------------------------------------------------------------------
	
	//==========================Card Reveal=============================================================================
public:
	
protected:
	void CardRevealed();
	
	void RefreshBattleSkillUI();
	
	void HandleBattleSkillStateChanged();
	//------------------------------------------------------------------------------------------------------------------

	//==========================Battle Action Sequence==================================================================

public:
	void BattleActionSequenceStart();
	
	void DisplayBattleActionSequenceStartUIFinish();

	// 비동기 Reveal 연출을 시작했으면 true를 반환하고, 연출 종료 시 NotifyDeceiveCardRevealFinished()를 호출한다.
	UFUNCTION(BlueprintNativeEvent, Category = "Battle|Deceive")
	bool PlayDeceiveCardReveal(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* PresentedCard, UMuksiBattleCardDataAsset* ActualCard);
	virtual bool PlayDeceiveCardReveal_Implementation(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* PresentedCard, UMuksiBattleCardDataAsset* ActualCard);

	UFUNCTION(BlueprintCallable, Category = "Battle|Deceive")
	void NotifyDeceiveCardRevealFinished();

	void PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter, ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset);

	void BattleActionSequenceEnd();
	void DisplayBattleActionSequenceEndUIFinish();

protected:
	UPROPERTY(EditAnywhere)
	float TurnTime = 0.1f;
};
