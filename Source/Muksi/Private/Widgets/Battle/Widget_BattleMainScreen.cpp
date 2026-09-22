// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"


#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "TimerManager.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
//#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleControlPanel.h"
#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleSkillBar.h"
#include "Muksi/Widgets/Battle/CardPreview/CardPreviewPanel.h"
#include "Muksi/Widgets/Battle/Hand/Card/BattleCardManager.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"
#include "Muksi/Widgets/Battle/PipeLine/BattlePipelineWidget.h"
#include "Muksi/Widgets/Battle/SkillReveal/Widget_BattleSkillRevealPanel.h"
#include "Muksi/Widgets/Battle/StatusHUD/BattleStatusHUDWidget.h"


void UWidget_BattleMainScreen::NativeConstruct()
{
	Super::NativeConstruct();

	UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
	if (!ManagerSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldManagerSubsystem is nullptr (Widget_BattleMainScreen.cpp)"));
		return;
	}

	BattleManager = ManagerSubsystem->GetManager<ABattleManager>();
	if (!BattleManager)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleManager is nullptr (Widget_BattleMainScreen.cpp)"));
		return;
	}

	BattleTargetingManager = BattleManager->GetBattleTargetingManager();
	BattleSequenceManager = BattleManager->GetBattleSequenceManager();
	BattleSimulationManager = BattleManager->GetBattleSimulationManager();

	if (!BattleTargetingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleTargetingManager is nullptr (Widget_BattleMainScreen.cpp)"));
		return;
	}

	BattleTargetingManager->OnEnemyCardSelectionReady.AddUObject(this, &UWidget_BattleMainScreen::HandleEnemyCardSelectionReady);
	BattleTargetingManager->OnPlayerTargetingCancelled.AddUObject(this, &UWidget_BattleMainScreen::HandlePlayerTargetingCancelled);

	BindBattleManagerEvents();
	BindBattlePipelineWidgetEvents();
	
	
	BindBattleSkillReveal();
	BindBattleSequenceManagerEvents();
	BindBattleControlPanelEvents();

	BattleManager->StartBattleFlow();
	
	/*if (CardPreviewPanel)
	{
		CardPreviewPanel->HidePreview();
	}*/
}

void UWidget_BattleMainScreen::NativeDestruct()
{
	UnbindBattleManagerEvents();
	UnbindBattleSequenceManagerEvents();
	UnbindBattlePipelineWidgetEvents();
	UnbindBattleSkillEvent();
	
	UnbindBattleSkillReveal();
	UnbindBattleControlPanelEvents();

	if (BattleTargetingManager)
	{
		BattleTargetingManager->OnEnemyCardSelectionReady.RemoveAll(this);
		BattleTargetingManager->OnPlayerTargetingCancelled.RemoveAll(this);
	}

	Super::NativeDestruct();
}

FReply UWidget_BattleMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Debug::Print(TEXT("ActivatableBase MouseDown"));
	return FReply::Unhandled();
}

void UWidget_BattleMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	RequestRefreshFocus();
}


void UWidget_BattleMainScreen::SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	checkf(IsValid(Player), TEXT("PlayerCharacter is null"));
	checkf(IsValid(Enemy), TEXT("EnemyCharacter is null"));

	ActivePassiveWidget->SetData(Player, Enemy);//각 캐릭터 Passive 관련 위젯 설정
	StatusHUDWidget->SetData(Player, Enemy);//각 캐릭터 Stat 관련 위젯 설정
	
	/*//HandWidget 관련 기능은 없어질 예정
	HandWidget->SetBattleCharacter(Player);//BattleCharacterBase의 BattleCardId로 손패 관련 설정
	HandWidget->BindingBattleCardManager(BattleManager->GetBattleCardManager());//HandWidget과 BattleCardComponent 바인딩*/
	
	BattleControlPanel->SetBattleCharacter(Player);
	
	BindBattleSkillEvent();
}


void UWidget_BattleMainScreen::BindBattlePipelineWidgetEvents()
{
	BattlePipelineWidget->OnPresentationFinished.AddUObject(
		this,
		&UWidget_BattleMainScreen::HandlePipelineUIFinish
	);
}

void UWidget_BattleMainScreen::UnbindBattlePipelineWidgetEvents()
{
	BattlePipelineWidget->OnPresentationFinished.RemoveAll(this);
}

void UWidget_BattleMainScreen::BindBattleManagerEvents()
{
	if (!BattleManager)
	{
		return;
	}

	BattleManager->PhaseUIRequestedDelegate.AddUniqueDynamic(this, &UWidget_BattleMainScreen::HandlePhaseUIRequested);
}

void UWidget_BattleMainScreen::UnbindBattleManagerEvents()
{
	if (!BattleManager)
	{
		return;
	}

	BattleManager->PhaseUIRequestedDelegate.RemoveDynamic(this, &UWidget_BattleMainScreen::HandlePhaseUIRequested);
	PhaseUITask = nullptr;
}

void UWidget_BattleMainScreen::BindBattleSequenceManagerEvents()
{
	if (!BattleSequenceManager)
		return;

	BattleSequenceManager->DeceiveCardRevealRequestedDelegate.AddUObject(this, &UWidget_BattleMainScreen::HandleDeceiveCardRevealRequested);
}

void UWidget_BattleMainScreen::UnbindBattleSequenceManagerEvents()
{
	if (!BattleSequenceManager)
		return;

	BattleSequenceManager->DeceiveCardRevealRequestedDelegate.RemoveAll(this);
}

void UWidget_BattleMainScreen::BindBattleSkillReveal()
{
	if (BattleSkillRevealPanel)
	{
		//스킬 공개
		BattleSkillRevealPanel->OnSkillRevealFinished.RemoveAll(this);
		BattleSkillRevealPanel->OnSkillRevealFinished.AddUObject(this, &UWidget_BattleMainScreen::HandleSkillRevealFinished);
		
		//변초 공개
		BattleSkillRevealPanel->OnDeceiveRevealFinished.RemoveAll(this);
		BattleSkillRevealPanel->OnDeceiveRevealFinished.AddUObject(this, &UWidget_BattleMainScreen::NotifyDeceiveCardRevealFinished);
	}
}

void UWidget_BattleMainScreen::UnbindBattleSkillReveal()
{
	if (BattleSkillRevealPanel)
	{
		BattleSkillRevealPanel->OnSkillRevealFinished.RemoveAll(this);
		BattleSkillRevealPanel->OnDeceiveRevealFinished.RemoveAll(this);
	}
	
}

void UWidget_BattleMainScreen::BindBattleSkillEvent()
{
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return;
	}

	ABattleCharacterBase* Player = RuntimeContext->GetPlayerCharacter();
	if (!Player)
	{
		return;
	}
	UBattleSkillComponent* SkillComponent = Player->GetBattleSkillComponent();
	if (!SkillComponent)
	{
		return;
	}
	SkillComponent->OnBattleSkillStateChanged.RemoveAll(this);
	SkillComponent->OnBattleSkillStateChanged.AddUObject(this, &UWidget_BattleMainScreen::HandleBattleSkillStateChanged);
}

void UWidget_BattleMainScreen::UnbindBattleSkillEvent()
{
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return;
	}

	ABattleCharacterBase* Player = RuntimeContext->GetPlayerCharacter();

	if (!Player)
	{
		return;
	}

	UBattleSkillComponent* SkillComponent = Player->GetBattleSkillComponent();

	if (!SkillComponent)
	{
		return;
	}

	SkillComponent->OnBattleSkillStateChanged.RemoveAll(this);
}

void UWidget_BattleMainScreen::BindBattleControlPanelEvents()
{
	if (!BattleControlPanel)
	{
		return;
	}

	BattleControlPanel->OnBattleSkillSelected.RemoveAll(this);

	BattleControlPanel->OnBattleSkillSelected.AddUObject(this,&UWidget_BattleMainScreen::HandleBattleSkillSelected);
	
	UWidget_BattleSkillBar* BattleSkillBar = BattleControlPanel->GetBattleSkillBar();
	BattleSkillBar->OnBattleSkillHovered.RemoveAll(
	CardPreviewPanel);

	BattleSkillBar->OnBattleSkillHovered.AddUObject(CardPreviewPanel, &UCardPreviewPanel::HandleSkillHovered);

	BattleSkillBar->OnBattleSkillUnhovered.RemoveAll(CardPreviewPanel);

	BattleSkillBar->OnBattleSkillUnhovered.AddUObject(CardPreviewPanel,&UCardPreviewPanel::HandleSkillHoverEnded);

}

void UWidget_BattleMainScreen::UnbindBattleControlPanelEvents()
{
	if (!BattleControlPanel)
	{
		return;
	}

	BattleControlPanel->OnBattleSkillSelected.RemoveAll(this);
}

void UWidget_BattleMainScreen::HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	if (!InstanceId.IsValid() || !CardData)
	{
		return;
	}

	BattleTargetingManager->RequestPlayerSkillSelection(InstanceId, CardData);
}

void UWidget_BattleMainScreen::HandleBattleSkillHovered(UMuksiBattleCardDataAsset* SkillData)
{
	/*if (!CardInfoPanel || !SkillData)
	{
		return;
	}

	CardInfoPanel->SetCardData(SkillData);
	CardInfoPanel->SetVisibility(
		ESlateVisibility::HitTestInvisible
	);*/
}

void UWidget_BattleMainScreen::HandleBattleSkillUnhovered()
{
	
}

void UWidget_BattleMainScreen::HandlePlayerTargetingCancelled()
{
	
}

void UWidget_BattleMainScreen::BattlePipelineWidgetSetting(EBattlePhase BattlePhase)
{
	HandleUIFinishCount += 1;
	
	FBattlePhasePresentationContext PhaseContext;
	PhaseContext.Phase = BattlePhase;
	PhaseContext.RoundNum = BattleManager->GetCurrentRound();
	PhaseContext.ExchangeNum = BattleManager->GetCurrentExchange();
	
	BattlePipelineWidget->DisplayPhase(PhaseContext);
}

void UWidget_BattleMainScreen::HandleDeceiveCardRevealRequested(const FBattleAction& BattleAction)
{
	if (!BattleSequenceManager || !IsValid(BattleAction.Card))
		return;

	UMuksiBattleCardDataAsset* ActualCard = BattleAction.Card->GetActualCard();
	if (!IsValid(ActualCard))
	{
		BattleSequenceManager->NotifyDeceiveCardRevealFinished();
		return;
	}

	if (!PlayDeceiveCardReveal(BattleAction, BattleAction.Card.Get(), ActualCard))
		BattleSequenceManager->NotifyDeceiveCardRevealFinished();
}

void UWidget_BattleMainScreen::HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	PhaseUITask = nullptr;
	
	if (NewPhase != EBattlePhase::CardReveal && TaskContext) 
		PhaseUITask = TaskContext->RegisterTask(this);

	switch (NewPhase)
	{
	case EBattlePhase::ReadyStart:
		ReadyStart();
		CompletePhaseUI(EBattlePhase::ReadyStart);
		break;

	case EBattlePhase::ReadyEnd:
		ReadyEnd();
		CompletePhaseUI(EBattlePhase::ReadyEnd);
		break;

	case EBattlePhase::BattleStart:
		BattleStart();
		break;

	case EBattlePhase::RoundStart:
		RoundStart();
		break;

	case EBattlePhase::ExchangeStart:
		ExchangeStart();
		break;

	case EBattlePhase::CardSelect:
		if (OldPhase != EBattlePhase::Targeting)
		{
			StartExchangeSelectCard(BattleManager->GetCurrentExchange());
		}
		CompletePhaseUI(EBattlePhase::CardSelect);
		break;

	case EBattlePhase::Targeting:
		CompletePhaseUI(EBattlePhase::Targeting);
		break;

	case EBattlePhase::CardReveal:
		CardRevealed();
		break;

	case EBattlePhase::SimulationSequence:
		FinishExchange(BattleManager->GetCurrentExchange());
		CompletePhaseUI(EBattlePhase::SimulationSequence);
		break;

	case EBattlePhase::ExchangeEnd:
		ExchangeEnd();
		break;

	case EBattlePhase::BattleActionSequenceStart:
		BattleActionSequenceStart();
		break;

	case EBattlePhase::BattleActionSequenceEnd:
		BattleActionSequenceEnd();
		break;

	case EBattlePhase::RoundEnd:
		RoundEnd();
		break;

	case EBattlePhase::BattleEnd:
		BattleEnd();
		break;

	default:
		CompletePhaseUI(NewPhase);
		break;
	}
}

void UWidget_BattleMainScreen::CompletePhaseUI(EBattlePhase FinishedPhase)
{
	if (!BattleManager || BattleManager->GetCurrentPhase() != FinishedPhase) 
		return;
	UBattlePhaseTask* CompletedTask = PhaseUITask;
	PhaseUITask = nullptr;
	if (CompletedTask) 
		CompletedTask->Complete();
}


void UWidget_BattleMainScreen::NotifyPlayerCardUnequipped()
{
	if (BattleTargetingManager)
	{
        BattleTargetingManager->CancelPlayerTargeting();
	}
}



bool UWidget_BattleMainScreen::CanRequestEndExchange()
{
	if (!BattleManager)
	{
		return false;
	}

	/*if (!HandWidget)
	{
		return false;
	}*/

	switch (BattleManager->GetCurrentPhase())
	{
	case EBattlePhase::ExchangeStart:
	case EBattlePhase::CardSelect:
	case EBattlePhase::Targeting:
	case EBattlePhase::CardReveal:
	case EBattlePhase::SimulationSequence:
		break;

	default:
		return false;
	}

	const int32 ExchangeNumber = BattleManager->GetCurrentExchange();

	if (ExchangeNumber < 0 || ExchangeNumber >= BattleManager->GetMaxExchangeCount())
	{
		return false;
	}

	return true;
}



void UWidget_BattleMainScreen::HandlePipelineUIFinish()
{
	switch (BattleManager->GetCurrentPhase())
	{
		case EBattlePhase::None:
		UE_LOG(LogTemp, Error, TEXT("Current Phase is Error (Widget_BattleMainScreen.cpp)"));
		break;
	case EBattlePhase::BattleStart:
		DisplayBattleStartUIFinish();
		break;
	case EBattlePhase::RoundStart:
		DisplayRoundStartUIFinish();
		break;
	case EBattlePhase::ExchangeStart:
		DisplayExchangeStartUIFinish();
		break;
	case EBattlePhase::ExchangeEnd:
		DisplayExchangeEndUIFinish();
		break;
	case EBattlePhase::BattleActionSequenceStart:
		DisplayBattleActionSequenceStartUIFinish();
		break;
	case EBattlePhase::BattleActionSequenceEnd:
		DisplayBattleActionSequenceEndUIFinish();
		break;
	case EBattlePhase::RoundEnd:
		DisplayRoundEndUIFinish();
		break;
	case EBattlePhase::BattleEnd:
		DisplayBattleEndUIFinish();
		break;
	default:
		break;
	}
}

void UWidget_BattleMainScreen::ReadyStart()
{
	// UI 받아올 거 설정
	//위젯 -> Widget_BattleMainScreen 바인딩
	if (BattleControlPanel)
	{
		BattleControlPanel->OnExchangeTimeExpired.AddUObject(this, &UWidget_BattleMainScreen::TimeOutCardSelect);
	}
}

void UWidget_BattleMainScreen::ReadyEnd()
{
	// ReadyEnd 때 SetupManager가 RuntimeContext에 기록한 BattleCharacter 정보를 사용한다.
	UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
	if (!BattleRuntimeContext)
	{
		return;
	}

	SetCharacterData(BattleRuntimeContext->GetPlayerCharacter(), BattleRuntimeContext->GetEnemyCharacter());
}


//==========================================전투 시작(Battle)============================================================
void UWidget_BattleMainScreen::BattleStart()
{
	HandleUIFinishCount = 0;
	//전투 시작 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::BattleStart);
}

void UWidget_BattleMainScreen::DisplayBattleStartUIFinish()
{
	// 모든 BattleStart 연출이 끝나면 UI 완료만 통지하고, 다음 Phase는 BattleManager가 결정한다.
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleStart);
	}
}

void UWidget_BattleMainScreen::BattleEnd()
{
	//Pipeline UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::BattleEnd);
}

void UWidget_BattleMainScreen::DisplayBattleEndUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleEnd);
	}
}

void UWidget_BattleMainScreen::HandleBattleStartFinish()
{
	CompletePhaseUI(EBattlePhase::BattleStart);
}

void UWidget_BattleMainScreen::HandleBattleEndFinish()
{
	CompletePhaseUI(EBattlePhase::BattleEnd);
}

//=========================================국 시작(Round)================================================================
void UWidget_BattleMainScreen::RoundStart()
{
	HandleUIFinishCount = 0;
	
	// Round 단위로 한 번만 핸드와 기존 선택 카드 표시를 초기화한다.
	//SetBattleCardToHand();//HandWidget의 카드 데이터 확인 후 드로우, 카드 활성화
	SetBattleSkillSetting();
	//국 시작 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::RoundStart);
}


void UWidget_BattleMainScreen::DisplayRoundStartUIFinish()
{
	HandleUIFinishCount -= 1;

	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::RoundStart);
	}
}



void UWidget_BattleMainScreen::RoundEnd()
{
	HandleUIFinishCount = 0;

	if (BattleSkillRevealPanel)
	{
		BattleSkillRevealPanel->ClearAllSkills();
	}
	
	//국 종료 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::RoundEnd);
}


void UWidget_BattleMainScreen::DisplayRoundEndUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::RoundEnd);
	}
}

void UWidget_BattleMainScreen::HandleRoundEndFinish()
{
	CompletePhaseUI(EBattlePhase::RoundEnd);
}

//------------------------------------------합 시작(Exchange)------------------------------------------------------------
void UWidget_BattleMainScreen::ExchangeStart()
{
	if (!BattleManager || !BattleSkillRevealPanel || !BattleControlPanel)
	{
		return;
	}
	
	HandleUIFinishCount = 0;
	
	BattlePipelineWidgetSetting(EBattlePhase::ExchangeStart);
}


void UWidget_BattleMainScreen::DisplayExchangeStartUIFinish()
{
	if (HandleUIFinishCount > 0)
		HandleUIFinishCount--;
	if (HandleUIFinishCount > 0)
	{
		return;
	}

	CompletePhaseUI(EBattlePhase::ExchangeStart);
}

void UWidget_BattleMainScreen::StartExchangeSelectCard(int32 ExchangeIndex)
{
	if (!BattleManager || ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	HandleSkillSlot(true);

	if (BattleTargetingManager)
	{
		BattleTargetingManager->RequestEnemyCardSelection();
	}

	HandleUIFinishCount = 0;

	if (BattleControlPanel)
	{
		BattleControlPanel->StartExchangeTimer(ExchangeIndex);
	}

	BattlePipelineWidgetSetting(EBattlePhase::CardSelect);
}

void UWidget_BattleMainScreen::FinishExchange(int32 ExchangeIndex)
{
	if (!BattleManager || ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	HandleUIFinishCount = 0;
}
void UWidget_BattleMainScreen::ExchangeEnd()
{
	HandleUIFinishCount = 0;
	
	//합 종료 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::ExchangeEnd);
}


void UWidget_BattleMainScreen::DisplayExchangeEndUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::ExchangeEnd);
	}
}

void UWidget_BattleMainScreen::HandleExchangeEndFinish()
{
	HandleUIFinishCount--;
	if (HandleUIFinishCount > 0)
	{
		return;
	}
	CompletePhaseUI(EBattlePhase::ExchangeEnd);
}

void UWidget_BattleMainScreen::HandleSkillSlot(bool bActive)
{
	BattleControlPanel->GetBattleSkillBar()->SetSkillSlotActive(bActive);
}




void UWidget_BattleMainScreen::SetBattleCardToHand()
{
	/*if (!HandWidget)
	{
		return;
	}
	
	HandWidget->RemoveSelectedCardsData();
	if (!HandWidget->HasHandCardWidgets())
	{
		HandWidget->DrawCards();
	}
	else
	{
		HandWidget->VisibleHandCards();
	}

	//TODO 카드 변화 기능이 예약되어 있으면 해당 카드 변화 효과 적용
	//TODO 이 기능이 적용되어있으면 HandlePipelineUIFinish()로 연결
	
	HandWidget->HitActiveHandCards(true);
	
	// 추가*/
	
}

void UWidget_BattleMainScreen::SetBattleSkillSetting()
{
	//지금 당장은 Skill Slot에 있는 쿨 다운 패널 초기화
	if (BattleControlPanel)
	{
		BattleControlPanel->RefreshSkillSlots();
	}
}



void UWidget_BattleMainScreen::HandleSkillRevealFinished(int32 ExchangeIndex)
{
	if (!BattleManager ||
		!BattleTargetingManager ||
		BattleManager->GetCurrentPhase() != EBattlePhase::CardReveal ||
		ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	BattleTargetingManager->NotifyEnemyCardRevealUIFinished(ExchangeIndex);
}

void UWidget_BattleMainScreen::TimeOutCardSelect()
{
	if (!BattleManager || !BattleTargetingManager)
    {
        return;
    }

    const EBattlePhase CurrentPhase = BattleManager->GetCurrentPhase();

    if (CurrentPhase != EBattlePhase::CardSelect &&
        CurrentPhase != EBattlePhase::Targeting)
    {
        return;
    }

    UBattleCardManager* BattleCardManager = BattleManager->GetBattleCardManager();

    UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

    if (!BattleCardManager || !RuntimeContext)
    {
        return;
    }

    const int32 CurrentExchange = BattleManager->GetCurrentExchange();

    const bool bPlayerNeedsPanic = RuntimeContext->GetPlayerExchangeAction(CurrentExchange) == nullptr;

    const bool bEnemyNeedsPanic = RuntimeContext->GetEnemyExchangeAction(CurrentExchange) == nullptr;

    if (!bPlayerNeedsPanic && !bEnemyNeedsPanic)
    {
        return;
    }

    // Enemy가 AI 선택 대기 중이라면 중단
    if (bEnemyNeedsPanic)
    {
        BattleTargetingManager->CancelPendingEnemyCardSelection();
    }

    FCharacterPanicTimeoutResult PlayerResult;
    FCharacterPanicTimeoutResult EnemyResult;

    bool bPlayerResolved = false;
    bool bEnemyResolved = false;

    if (bPlayerNeedsPanic)
    {
        bPlayerResolved =
            BattleCardManager->ResolvePlayerPanicOnTimeout(
                CurrentExchange,
                PlayerResult);

        if (!bPlayerResolved)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Failed to resolve player panic"));
        }
    }

    if (bEnemyNeedsPanic)
    {
        bEnemyResolved =
            BattleCardManager->ResolveEnemyPanicOnTimeout(
                CurrentExchange,
                EnemyResult);

        if (!bEnemyResolved)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Failed to resolve enemy panic"));
        }
    }

    // Player Panic UI 표시
    if (bPlayerResolved)
    {
    	// TODO: Player Panic 발생 관련 UI 처리

        if (!BattleTargetingManager->RequestPlayerPanicTargeting(PlayerResult.PanicCard))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Failed player panic targeting"));
        }
    }

    if (bEnemyResolved)
    {
    	// TODO: Enemy Panic 발생 관련 UI 처리
        if (!BattleTargetingManager->RequestEnemyPanicTargeting(EnemyResult.PanicCard))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Failed enemy panic targeting"));
        }
    }
}

bool UWidget_BattleMainScreen::UpdateCurrentExchangeSkillReveal()
{
	if (!BattleManager || !BattleSkillRevealPanel)
	{
		return false;
	}

	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return false;
	}

	
	const int32 ExchangeIndex = BattleManager->GetCurrentExchange();

	const FBattleAction* PlayerAction = RuntimeContext->GetPlayerExchangeAction(ExchangeIndex);

	const FBattleAction* EnemyAction = RuntimeContext->GetEnemyExchangeAction(ExchangeIndex);

	if (!PlayerAction || !EnemyAction)
	{
		return false;
	}

	const bool bPlayerFirst = PlayerAction->IsHigherPriorityThan(*EnemyAction);

	const FBattleAction* FirstAction =bPlayerFirst ? PlayerAction : EnemyAction;

	const FBattleAction* SecondAction = bPlayerFirst ? EnemyAction : PlayerAction;
	
	return BattleSkillRevealPanel->RevealExchangeSkills(
	ExchangeIndex,
	FirstAction->Card,
	FirstAction->bPlayerAction,
	SecondAction->Card,
	SecondAction->bPlayerAction
		);	
}

void UWidget_BattleMainScreen::HandleEnemyCardSelectionReady(
	UMuksiBattleCardDataAsset* EnemyCard,
	int32 ExchangeIndex)
{
	if (!BattleManager
		|| !BattleTargetingManager
		|| ExchangeIndex != BattleManager->GetCurrentExchange()
		|| !EnemyCard)
	{
		return;
	}

	BattleTargetingManager->NotifyEnemyCardSelectionUIFinished();
}




void UWidget_BattleMainScreen::CardRevealed() 
{
	if (!BattleManager)
	{
		return;
	}

	if (BattleControlPanel)
	{
		BattleControlPanel->StopExchangeTimer();
	}

	const int32 ExchangeIndex = BattleManager->GetCurrentExchange();

	if (!UpdateCurrentExchangeSkillReveal())
	{
		HandleSkillRevealFinished(ExchangeIndex);
	}
}


void UWidget_BattleMainScreen::RefreshBattleSkillUI()
{
	if (!BattleControlPanel)
	{
		return;
	}

	BattleControlPanel->RefreshSkillBar();
}

void UWidget_BattleMainScreen::HandleBattleSkillStateChanged()
{
	if (!BattleControlPanel)
	{
		return;
	}

	BattleControlPanel->RefreshSkillBar();
}

//----------------------------------------------------------------------------------------------------------------------
//------------------------------------Battle Action Sequence------------------------------------------------------------

void UWidget_BattleMainScreen::BattleActionSequenceStart()
{
	HandleUIFinishCount = 0;

	if (!BattleControlPanel || !BattleManager)
	{
		return;
	}
	
	//HandWidget 관련 설정 <- 따로 빼서 함수 하나만 적을예정-----------------------------
	if (!BattleControlPanel)
	{
		return;
	}

	BattleControlPanel->GetBattleSkillBar()->SetSkillSlotActive(false);
	/*// Exchange가 끝났으므로 더 이상 카드 선택 불가
	HandWidget->HitActiveHandCards(false);
	// 남아 있는 손패를 아래쪽 대기 위치로 이동
	HandWidget->InvisibleHandCards();
	//-------------------------------------------------------------------------------*/

	//공격 시작 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::BattleActionSequenceStart);
}


void UWidget_BattleMainScreen::DisplayBattleActionSequenceStartUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleActionSequenceStart);
	}
}

bool UWidget_BattleMainScreen::PlayDeceiveCardReveal_Implementation(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* PresentedCard, UMuksiBattleCardDataAsset* ActualCard)
{
	if (!BattleSkillRevealPanel  || !IsValid(ActualCard))
		return false;

	
	return BattleSkillRevealPanel->PlayDeceiveReveal(BattleAction.ExchangeIndex, BattleAction.bPlayerAction, ActualCard);
}

void UWidget_BattleMainScreen::NotifyDeceiveCardRevealFinished()
{
	if (!BattleSequenceManager)
	{
		return;
	}

	BattleSequenceManager->NotifyDeceiveCardRevealFinished();
}

void UWidget_BattleMainScreen::PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter, ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset)
{

}


void UWidget_BattleMainScreen::BattleActionSequenceEnd()
{
	HandleUIFinishCount = 0;

	//UI 기능적으로 일단 뭐 없음
	BattlePipelineWidgetSetting(EBattlePhase::BattleActionSequenceEnd);
}


void UWidget_BattleMainScreen::DisplayBattleActionSequenceEndUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleActionSequenceEnd);
	}
}

//----------------------------------------------------------------------------------------------------------------------
