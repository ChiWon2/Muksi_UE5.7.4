// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"


#include "Muksi/Widgets/Battle/Hand/HandWidget.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "TimerManager.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Hand/ExchangeControl/ExchangeControlWidget.h"
#include "Muksi/Widgets/Battle/Hand/ExchangeSlot/ExchangeSlotPanelWidget.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"
#include "Muksi/Widgets/Battle/PipeLine/BattlePipelineWidget.h"
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

	BindBattleManagerEvents();
	BindBattleSequenceManagerEvents();
	BindBattlePipelineWidgetEvents();
	BindHandWidgetEvents();
	

	BattleManager->StartBattleFlow();
}

void UWidget_BattleMainScreen::NativeDestruct()
{
	UnbindBattleManagerEvents();
	UnbindBattleSequenceManagerEvents();
	UnbindBattlePipelineWidgetEvents();
	UnbindHandWidgetEvents();

	if (BattleTargetingManager)
	{
		BattleTargetingManager->OnEnemyCardSelectionReady.RemoveAll(this);
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
	HandWidget->SetBattleCharacter(Player);//BattleCharacterBase의 BattleCardId로 손패 관련 설정
}


void UWidget_BattleMainScreen::BindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}
	HandWidget->OnPlayerCardReturned.RemoveAll(this);
	HandWidget->OnPlayerCardReturned.AddUObject(this, &UWidget_BattleMainScreen::NotifyPlayerCardUnequipped);
	if (UExchangeSlotPanelWidget* ExchangePanel = HandWidget->GetExchangeSlotPanelWidget())
	{
		ExchangePanel->OnEnemyCardRevealFinished.AddUObject(this, &UWidget_BattleMainScreen::HandleEnemyCardRevealFinished);
	}
	
	//아래 카드 선택 버튼은 나중에 따로 빼기 
	ExchangeControlWidget->OnEndTurnRequested.AddUniqueDynamic(this, &UWidget_BattleMainScreen::HandleCardSelect);
}

void UWidget_BattleMainScreen::UnbindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}
	HandWidget->OnPlayerCardReturned.RemoveAll(this);
	if (UExchangeSlotPanelWidget* ExchangePanel = HandWidget->GetExchangeSlotPanelWidget())
	{
		ExchangePanel->OnEnemyCardRevealFinished.RemoveAll(this);
	}
	
	//아래 카드 선택 버튼은 나중에 따로 빼기
	ExchangeControlWidget->OnEndTurnRequested.RemoveDynamic(this, &UWidget_BattleMainScreen::HandleCardSelect);
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
	{
		return;
	}

	BattleSequenceManager->DeceiveCardRevealRequestedDelegate.AddUObject(this, &UWidget_BattleMainScreen::HandleDeceiveCardRevealRequested);
}

void UWidget_BattleMainScreen::UnbindBattleSequenceManagerEvents()
{
	if (!BattleSequenceManager)
	{
		return;
	}

	BattleSequenceManager->DeceiveCardRevealRequestedDelegate.RemoveAll(this);
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
	{
		return;
	}

	UMuksiBattleCardDataAsset* DeceivedCard = BattleAction.Card->GetDeceivedCard();
	if (!IsValid(DeceivedCard))
	{
		BattleSequenceManager->NotifyDeceiveCardRevealFinished();
		return;
	}

	if (!PlayDeceiveCardReveal(BattleAction, DeceivedCard, BattleAction.Card.Get()))
	{
		BattleSequenceManager->NotifyDeceiveCardRevealFinished();
	}
}

void UWidget_BattleMainScreen::HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	(void)OldPhase;
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
		StartExchangeSelectCard(BattleManager->GetCurrentExchange());
		CompletePhaseUI(EBattlePhase::CardSelect);
		break;

	case EBattlePhase::Targeting:
		CompletePhaseUI(EBattlePhase::Targeting);
		break;

	case EBattlePhase::CardReveal:
		if (!RevealEnemySelectedCard(BattleManager->GetCurrentExchange()) && BattleTargetingManager)
		{
			BattleTargetingManager->NotifyEnemyCardRevealUIFinished(BattleManager->GetCurrentExchange());
		}
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

bool UWidget_BattleMainScreen::SetSimulationPlayerView(EBattlePlayerSimulationView View)
{
	UBattleSimulationPresentationController* PresentationController = BattleSimulationManager ? BattleSimulationManager->GetPresentationController() : nullptr;
	return PresentationController ? PresentationController->RequestPlayerSimulationView(View) : false;
}

bool UWidget_BattleMainScreen::ToggleSimulationPlayerView()
{
	UBattleSimulationPresentationController* PresentationController = BattleSimulationManager ? BattleSimulationManager->GetPresentationController() : nullptr;
	return PresentationController ? PresentationController->TogglePlayerSimulationView() : false;
}

EBattlePlayerSimulationView UWidget_BattleMainScreen::GetSimulationPlayerView() const
{
	UBattleSimulationPresentationController* PresentationController = BattleSimulationManager ? BattleSimulationManager->GetPresentationController() : nullptr;
	return PresentationController ? PresentationController->GetPlayerSimulationView() : EBattlePlayerSimulationView::ActualSelf;
}

bool UWidget_BattleMainScreen::CanToggleSimulationPlayerView() const
{
	UBattleSimulationPresentationController* PresentationController = BattleSimulationManager ? BattleSimulationManager->GetPresentationController() : nullptr;
	return PresentationController ? PresentationController->CanChangePlayerSimulationView() : false;
}

bool UWidget_BattleMainScreen::CanRequestEndExchange()
{
	if (!BattleManager)
	{
		return false;
	}

	if (!HandWidget)
	{
		return false;
	}

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
	DisplayBattleStartUI();
}

void UWidget_BattleMainScreen::DisplayBattleStartUI()
{
	//전투 시작 UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::BattleStart);

	//뭐 다른 UI 표시 있으면 추가하고 ex) 전투 시작 시 카드 변경 효과같은 경우 HandWidget에 할당
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleStartUIFinish() 호출하기
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
	DisplayBattleEndUI();
}

void UWidget_BattleMainScreen::DisplayBattleEndUI()
{
	//전투 종료 UI 표시

	//Pipeline UI 표시
	BattlePipelineWidgetSetting(EBattlePhase::BattleEnd);
	
	//일단 그냥 넘기기
	//DisplayBattleEndUIFinish();
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
	//-----------------------------------------------------------------
	//Gameplay를 담당하는 Manager의 RoundStart로 옮길 예정-----------------
	//BattleCharacterBase의 카드 데이터를 관리 기능
	if (!BattleManager)
	{
		return;
	}
	UBattleRuntimeContext* BattleRuntimeContext =
		BattleManager->GetBattleRuntimeContext();

	if (!BattleRuntimeContext)
	{
		return;
	}

	ABattleCharacterBase* PlayerBattleCharacter =
		BattleRuntimeContext->GetPlayerCharacter();

	ABattleCharacterBase* EnemyBattleCharacter =
		BattleRuntimeContext->GetEnemyCharacter();

	if (!PlayerBattleCharacter || !EnemyBattleCharacter)
	{
		return;
	}
	
	if (UBattleCardComponent* PlayerCardComponent =
		PlayerBattleCharacter->GetBattleCardComponent())
	{
		PlayerCardComponent->RefillHandIfEmpty();
	}

	if (UBattleCardComponent* EnemyCardComponent =
		EnemyBattleCharacter->GetBattleCardComponent())
	{
		EnemyCardComponent->RefillHandIfEmpty();
	}
	//-----------------------------------------------------------------
	// Round 단위로 한 번만 핸드와 기존 선택 카드 표시를 초기화한다.
	SetBattleCardToHand();//HandWidget의 카드 데이터 확인 후 드로우, 카드 활성화
	DisplayRoundStartUI();
}

void UWidget_BattleMainScreen::DisplayRoundStartUI()
{
	//국 시작 UI 표시

	//InkLine 호출
	BattlePipelineWidgetSetting(EBattlePhase::RoundStart);
	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleStartUIFinish() 호출하기
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
	//이건 BattleCharacterBase의 내용을 수정하는 거라 여기 있으면 안됨 (일단 여기)
	if (BattleManager)
	{
		UBattleRuntimeContext* BattleRuntimeContext =
			BattleManager->GetBattleRuntimeContext();

		if (BattleRuntimeContext)
		{
			ABattleCharacterBase* PlayerBattleCharacter =
				BattleRuntimeContext->GetPlayerCharacter();

			ABattleCharacterBase* EnemyBattleCharacter =
				BattleRuntimeContext->GetEnemyCharacter();

			if (PlayerBattleCharacter)
			{
				if (UBattleCardComponent* CardComponent =
					PlayerBattleCharacter->GetBattleCardComponent())
				{
					CardComponent->ConsumeCommittedCards();
				}
			}

			if (EnemyBattleCharacter)
			{
				if (UBattleCardComponent* CardComponent =
					EnemyBattleCharacter->GetBattleCardComponent())
				{
					CardComponent->ConsumeCommittedCards();
				}
			}
		}
	}
	//---------------------------------------------------------
	
	// Exchange Slot 비우기
	ClearExchangeSlots();
	// Round 동안 유지한 핸드를 Round 종료 시 정리한다.
	ClearBattleCard();

	DisplayRoundEndUI();
}


void UWidget_BattleMainScreen::ClearExchangeSlots() const
{
	HandWidget->ClearPlayerSelectCard();
	HandWidget->ClearEnemySelectCard();
}

void UWidget_BattleMainScreen::DisplayRoundEndUI()
{
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
	if (!HandWidget || !BattleManager)
	{
		return;
	}
	ExchangeControlWidget->ShowSelectButton(true);// 턴 종료 버튼 활성화 (없어질 예정)
	HandleUIFinishCount = 0;
	
	DisplayExchangeStartUI();
}

void UWidget_BattleMainScreen::DisplayExchangeStartUI()
{
	//합 시작 UI 표시

	//InkLine 호출
	BattlePipelineWidgetSetting(EBattlePhase::ExchangeStart);

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleRoundUIFinish() 호출하기
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

	const int32 ExchangeNumber = ExchangeIndex + 1;

	HandleExchangeSlot(ExchangeNumber, true);

	// 적 카드 선택 타이밍과 Targeting 규칙은 BattleTargetingManager가 소유한다.
	if (BattleTargetingManager)
	{
		BattleTargetingManager->RequestEnemyCardSelection();
	}

	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시
	HandleUIFinishCount = 0;
	//Pipeline UI 표시
	
	BattlePipelineWidgetSetting(EBattlePhase::CardSelect);
}

void UWidget_BattleMainScreen::FinishExchange(int32 ExchangeIndex)
{
	if (!BattleManager || ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	HandleUIFinishCount = 0;
	HandleExchangeSlot(ExchangeIndex + 1, false);
}
void UWidget_BattleMainScreen::ExchangeEnd()
{
	HandleUIFinishCount = 0;
	
	ExchangeControlWidget->ShowSelectButton(false);// 턴 종료 버튼 비활성화 (없어질 예정)
	
	
	DisplayExchangeEndUI();
}

void UWidget_BattleMainScreen::DisplayExchangeEndUI()
{
	//합 종료 UI 표시

	//InkLine 표시
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

void UWidget_BattleMainScreen::HandleExchangeSlot(int32 Index, bool bActive)
{
	if (!HandWidget)
	{
		return;
	}

	UExchangeSlotPanelWidget* ExchangePanel =
		HandWidget->GetExchangeSlotPanelWidget();

	if (!ExchangePanel)
	{
		return;
	}

	ExchangePanel->EnableExchangeSlot(Index, bActive);
}

void UWidget_BattleMainScreen::HandleEnemyCardRevealFinished(int32 ExchangeIndex)
{
	if (!BattleTargetingManager)
	{
		return;
	}

	BattleTargetingManager->NotifyEnemyCardRevealUIFinished(ExchangeIndex);
}

void UWidget_BattleMainScreen::SetBattleCardToHand()
{
	if (!HandWidget)
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
}

void UWidget_BattleMainScreen::ClearBattleCard()const
{
	HandWidget->InvisibleHandCards();
	HandWidget->HitActiveHandCards(false);
}

void UWidget_BattleMainScreen::HandleEnemyCardSelectionReady(
	UMuksiBattleCardDataAsset* EnemyCard,
	int32 ExchangeIndex)
{
	if (!GetWorld()
		|| !BattleManager
		|| !BattleTargetingManager
		|| !HandWidget
		|| ExchangeIndex != BattleManager->GetCurrentExchange()
		|| !EnemyCard)
	{
		return;
	}

	UExchangeSlotPanelWidget* ExchangePanel = HandWidget->GetExchangeSlotPanelWidget();

	if (!ExchangePanel)
	{
		return;
	}

	ExchangePanel->PlaceEnemySelectCard(EnemyCard, ExchangeIndex);

	GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (BattleTargetingManager)
			{
				BattleTargetingManager
					->NotifyEnemyCardSelectionUIFinished();
			}
		})
	);
}

void UWidget_BattleMainScreen::HandleCardSelect()
{
	SelectCardDataSend();
}

void UWidget_BattleMainScreen::SelectCardDataSend()const
{
	if (!BattleManager || !BattleTargetingManager || !HandWidget)
	{
		return;
	}
	
	UExchangeSlotPanelWidget* ExchangePanel = HandWidget->GetExchangeSlotPanelWidget();
	if (!ExchangePanel)
	{
		return;
	}

	const int32 ExchangeNumber = BattleManager->GetCurrentExchange() + 1;
	
	const FCardEquipSlotData SlotData = ExchangePanel->GetSlotDataByExchangeNumber_Player(ExchangeNumber);
	if (SlotData.CardData)
	{
		BattleTargetingManager->RequestPlayerCardSelection(
			SlotData.CardData);
	}
}

bool UWidget_BattleMainScreen::RevealEnemySelectedCard(int32 ExchangeIndex)
{
	if (!HandWidget)
	{
		return false;
	}

	UExchangeSlotPanelWidget* ExchangePanel = HandWidget->GetExchangeSlotPanelWidget();
	if (!ExchangePanel)
	{
		return false;
	}
	
	return ExchangePanel->EnemySelectedBattleCardFlip(ExchangeIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
//------------------------------------Battle Action Sequence------------------------------------------------------------

void UWidget_BattleMainScreen::BattleActionSequenceStart()
{
	if (!HandWidget || !BattleManager)
	{
		return;
	}
	
	//HandWidget 관련 설정 <- 따로 빼서 함수 하나만 적을예정-----------------------------
	if (!HandWidget)
	{
		return;
	}

	// Exchange가 끝났으므로 더 이상 카드 선택 불가
	HandWidget->HitActiveHandCards(false);
	// 남아 있는 손패를 아래쪽 대기 위치로 이동
	HandWidget->InvisibleHandCards();
	//-------------------------------------------------------------------------------

	DisplayBattleActionSequenceStartUI();
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceStartUI()
{
	//합 시작 UI 표시

	//InkLine 호출
	BattlePipelineWidgetSetting(EBattlePhase::BattleActionSequenceStart);

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleRoundUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceStartUIFinish()
{
	HandleUIFinishCount -= 1;
	if (HandleUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleActionSequenceStart);
	}
}

bool UWidget_BattleMainScreen::PlayDeceiveCardReveal_Implementation(const FBattleAction& BattleAction, UMuksiBattleCardDataAsset* DeceivedCard, UMuksiBattleCardDataAsset* ActualCard)
{
	(void)BattleAction;
	(void)DeceivedCard;
	(void)ActualCard;
	return false;
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
	//UI 기능적으로 일단 뭐 없음
	DisplayBattleActionSequenceEndUI();
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceEndUI()
{
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
