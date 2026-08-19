// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Widget_BattleMainScreen.generated.h"

class UBattleStatusHUDWidget;
class UWidget_CharacterData;
class UHandWidget;
class UInkLineWidget;
class UPassiveActivePopupWidget;
class UMuksiBattleCardDataAsset;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattlePipelineWidget;
class UExchangeControlWidget;


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
	UHandWidget* HandWidget;

	UPROPERTY(meta = (BindWidget))
	UPassiveActivePopupWidget* ActivePassiveWidget;
	
	UPROPERTY(meta = (BindWidget))
	UBattleStatusHUDWidget* StatusHUDWidget;
	
	UPROPERTY(meta = (BindWidget))
	UBattlePipelineWidget* BattlePipelineWidget;
	
	UPROPERTY(meta = (BindWidget))
	UExchangeControlWidget* ExchangeControlWidget;

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
	void BindHandWidgetEvents();

	UFUNCTION(BlueprintCallable)
	void UnbindHandWidgetEvents();
	
	UFUNCTION(BlueprintCallable)
	void BindBattlePipelineWidgetEvents();
	
	UFUNCTION(BlueprintCallable)
	void UnbindBattlePipelineWidgetEvents();


	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool SetSimulationPlayerView(EBattlePlayerSimulationView View);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool ToggleSimulationPlayerView();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	EBattlePlayerSimulationView GetSimulationPlayerView() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	bool CanToggleSimulationPlayerView() const;

	bool CanRequestEndExchange();

	// 카드 슬롯에서 장착 해제될 때 진행 중인 타겟팅/대기 카드 상태를 함께 정리한다.
	void NotifyPlayerCardUnequipped();

	FTimerHandle InkLineTimerHandle;

	//---------------------------중앙 텍스트 블록 관련 함수---------------------------------------------------------------
public:
	void HandlePipelineUIFinish();

protected:
	//------------------------------------------------------------------------------------------------------------------
	void BindBattleManagerEvents();
	void UnbindBattleManagerEvents();
	void BindBattleSequenceManagerEvents();
	void UnbindBattleSequenceManagerEvents();

	UFUNCTION()
	void HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	void CompletePhaseUI(EBattlePhase FinishedPhase);

	void HandleDeceiveCardRevealRequested(const FBattleAction& BattleAction);

	//====================================Ready<준비>===================================================================
public:
	void ReadyStart();
	void ReadyEnd();

	//====================================Battle<전투>==================================================================
public:
	void BattleStart();

	void DisplayBattleStartUI();
	void DisplayBattleStartUIFinish();

	void BattleEnd();

	void DisplayBattleEndUI();
	void DisplayBattleEndUIFinish();

	void HandleBattleStartFinish();

	void HandleBattleEndFinish();
public:
	UPROPERTY(EditAnywhere)
	FString BattleStartText = "Battle Start!";
	UPROPERTY(EditAnywhere)
	FString BattleEndText = "Battle End!";
	int32 HandleBattleUIFinishCount = 0;

	//=====================================Round<국>====================================================================
public:
	//국 시작
	void RoundStart();

	void DisplayRoundStartUI();
	void DisplayRoundStartUIFinish();


	//국 종료
	void RoundEnd();

	void RemoveSelectCards()const;
	void ClearExchangeSlots()const;

	void DisplayRoundEndUI();
	void DisplayRoundEndUIFinish();

	void HandleRoundEndFinish();

public:
	UPROPERTY(EditAnywhere)
	FString RoundStartText = "Round Start!";
	UPROPERTY(EditAnywhere)
	FString RoundEndText = "Round End";

	UPROPERTY()
	int32 HandleRoundStartFinishCount = 0;
	UPROPERTY()
	int32 HandleRoundEndFinishCount = 0;

	//=================================Exchange<합>=====================================================================
	//합 시작	Round 시작

public:
	void ExchangeStart();

	void DisplayExchangeStartUI();
	void DisplayExchangeStartUIFinish();

	void DisplayExchangeEndUI();
	void DisplayExchangeEndUIFinish();

	void StartExchangeSelectCard(int32 ExchangeIndex);
	void FinishExchange(int32 ExchangeIndex);

	void ExchangeEnd();

	void HandleExchangeEndFinish();

protected:
	void HandleExchangeSlot(int32 Index, bool bActive);
	void HandleEnemyCardRevealFinished(int32 ExchangeIndex);
	void SetBattleCardToHand();
	void ClearBattleCard()const;
public:

	UPROPERTY()
	int32 HandleExchangeCount = 0;

	UPROPERTY(EditAnywhere)
	FString ExchangeStartText= "Exchange Start!";

	UPROPERTY(EditAnywhere)
	FString ExchangeCountText = "Exchange";

	UPROPERTY(EditAnywhere)
	FString ExchangeEndText = "Exchange End!";

	void HandleEnemyCardSelectionReady(UMuksiBattleCardDataAsset* EnemyCard, int32 ExchangeIndex);

public:
	//합 도중 카드 선택 확정 버튼
	UFUNCTION(BlueprintCallable)
	void HandleCardSelect();

	void SelectCardDataSend()const;


	bool RevealEnemySelectedCard(int32 ExchangeIndex);
	//------------------------------------------------------------------------------------------------------------------

	//==========================Battle Action Sequence==================================================================

public:
	void BattleActionSequenceStart();

	void DisplayBattleActionSequenceStartUI();
	void DisplayBattleActionSequenceStartUIFinish();

	// 비동기 Reveal 연출을 시작했으면 true를 반환하고, 연출 종료 시 NotifyDeceiveCardRevealFinished()를 호출한다.
	UFUNCTION(BlueprintNativeEvent, Category = "Battle|Deceive")
	bool PlayDeceiveCardReveal(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* DeceivedCard, UMuksiBattleCardDataAsset* ActualCard);
	virtual bool PlayDeceiveCardReveal_Implementation(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* DeceivedCard, UMuksiBattleCardDataAsset* ActualCard);

	UFUNCTION(BlueprintCallable, Category = "Battle|Deceive")
	void NotifyDeceiveCardRevealFinished();

	void PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter, ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset);

	void BattleActionSequenceEnd();
	void DisplayBattleActionSequenceEndUI();
	void DisplayBattleActionSequenceEndUIFinish();

protected:
	UPROPERTY(EditAnywhere)
	FString BattleActionSequenceStartText = "BattleAction Start!";
	UPROPERTY(EditAnywhere)
	FString BattleActionSequenceEndText = "BattleAction End!";
	UPROPERTY()
	int32 BattleActionSequenceUIFinishCount = 0;

	UPROPERTY(EditAnywhere)
	float TurnTime = 0.1f;
};
