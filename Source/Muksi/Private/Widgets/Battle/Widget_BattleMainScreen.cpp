// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"


#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "TimerManager.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"


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
	BindBattleSimulationManagerEvents();
	BindHandWidgetEvents();

	if (HandWidget)
		HandWidget->BattleMainScreen = this;

	BattleManager->StartBattleFlow();
}

void UWidget_BattleMainScreen::NativeDestruct()
{
	UnbindBattleManagerEvents();
	UnbindBattleSequenceManagerEvents();
	UnbindBattleSimulationManagerEvents();
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

	ActivePassiveWidget->SetData(Player, Enemy);
	SetPresentationCharacterData(Player, Enemy);
}

void UWidget_BattleMainScreen::SetPresentationCharacterData(ABattleCharacterBase* PlayerCharacter, ABattleCharacterBase* EnemyCharacter)
{
	if (!IsValid(PlayerCharacter) || !IsValid(EnemyCharacter))
		return;
	if (HandWidget)
		HandWidget->SetCharacterData(PlayerCharacter, EnemyCharacter);
}

void UWidget_BattleMainScreen::BindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}

	HandWidget->OnEndTurnRequested.AddUniqueDynamic(this, &UWidget_BattleMainScreen::HandleCardSelect);

	HandWidget->OnEnemyCardRevealFinished.AddUObject(this,&UWidget_BattleMainScreen::HandleEnemyCardRevealFinished);
}

void UWidget_BattleMainScreen::UnbindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}

	HandWidget->OnEndTurnRequested.RemoveDynamic(this, &UWidget_BattleMainScreen::HandleCardSelect);
	HandWidget->OnEnemyCardRevealFinished.RemoveAll(this);
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

void UWidget_BattleMainScreen::BindBattleSimulationManagerEvents()
{
	if (!BattleSimulationManager)
		return;

	BattleSimulationManager->PresentationCharactersChangedDelegate.AddUniqueDynamic(this, &UWidget_BattleMainScreen::HandleSimulationPresentationCharactersChanged);;
}

void UWidget_BattleMainScreen::UnbindBattleSimulationManagerEvents()
{
	if (!BattleSimulationManager) return;

	BattleSimulationManager->PresentationCharactersChangedDelegate.RemoveDynamic(this, &UWidget_BattleMainScreen::HandleSimulationPresentationCharactersChanged);
}

void UWidget_BattleMainScreen::HandleSimulationPresentationCharactersChanged(ABattleCharacterBase* PlayerCharacter, ABattleCharacterBase* EnemyCharacter)
{
	SetPresentationCharacterData(PlayerCharacter, EnemyCharacter);
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
		BattleTargetingManager->RequestCancelPlayerTargeting();
	}
}

bool UWidget_BattleMainScreen::SetSimulationPlayerView(EBattlePlayerSimulationView View)
{
	return BattleSimulationManager ? BattleSimulationManager->SetPlayerSimulationView(View) : false;
}

bool UWidget_BattleMainScreen::ToggleSimulationPlayerView()
{
	return BattleSimulationManager ? BattleSimulationManager->TogglePlayerSimulationView() : false;
}

EBattlePlayerSimulationView UWidget_BattleMainScreen::GetSimulationPlayerView() const
{
	return BattleSimulationManager ? BattleSimulationManager->GetPlayerSimulationView() : EBattlePlayerSimulationView::ActualSelf;
}

bool UWidget_BattleMainScreen::CanToggleSimulationPlayerView() const
{
	return BattleSimulationManager ? BattleSimulationManager->CanChangePlayerSimulationView() : false;
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
	DisplayBattleStartUI();
}

void UWidget_BattleMainScreen::DisplayBattleStartUI()
{
	//전투 시작 UI 표시

	//InkLine 호출
	HandWidget->DisplayInkLine(BattleStartText, TurnTime);
	HandleBattleUIFinishCount += 1;

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleStartUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayBattleStartUIFinish()
{
	// 모든 BattleStart 연출이 끝나면 UI 완료만 통지하고, 다음 Phase는 BattleManager가 결정한다.
	HandleBattleUIFinishCount -= 1;

	if (HandleBattleUIFinishCount <= 0)
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
	HandWidget->DisplayInkLine(BattleEndText, TurnTime);
	HandleBattleUIFinishCount += 1;

	//일단 그냥 넘기기
	//DisplayBattleEndUIFinish();
}

void UWidget_BattleMainScreen::DisplayBattleEndUIFinish()
{
	HandleBattleUIFinishCount -= 1;
	if (HandleBattleUIFinishCount <= 0)
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
	HandleRoundStartFinishCount = 0;

	// RemoveCard 배열 초기화
	HandWidget->RemoveSelectedCardsData();
	// Round 단위로 한 번만 핸드와 기존 선택 카드 표시를 초기화한다.
	SetBattleCardToHand();

	DisplayRoundStartUI();
}

void UWidget_BattleMainScreen::DisplayRoundStartUI()
{
	//국 시작 UI 표시

	//InkLine 호출
	FString ResultText = FString::Printf(TEXT("%d %s"), BattleManager->GetCurrentRound(), *RoundStartText);

	HandleRoundStartFinishCount += 1;
	HandWidget->DisplayInkLine(ResultText, TurnTime);

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleStartUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayRoundStartUIFinish()
{
	HandleRoundStartFinishCount -= 1;

	if (HandleRoundStartFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::RoundStart);
	}
}



void UWidget_BattleMainScreen::RoundEnd()
{
	// Exchange 단계에서 사용한 카드 묘지에 넣기
	RemoveSelectCards();
	// Exchange Slot 비우기
	ClearExchangeSlots();
	// Round 동안 유지한 핸드를 Round 종료 시 정리한다.
	ClearBattleCard();

	DisplayRoundEndUI();
}

void UWidget_BattleMainScreen::RemoveSelectCards()const
{
	if (!BattleManager)
	{
		return;
	}

	UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
	if (!BattleRuntimeContext)
	{
		return;
	}

	ABattleCharacterBase* PlayerBattleCharacter = BattleRuntimeContext->GetPlayerCharacter();
	ABattleCharacterBase* EnemyBattleCharacter = BattleRuntimeContext->GetEnemyCharacter();
	if (!PlayerBattleCharacter || !EnemyBattleCharacter)
	{
		return;
	}

	for (int32 ExchangeIndex = 0; ExchangeIndex < BattleManager->GetMaxExchangeCount(); ++ExchangeIndex)
	{
		const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
		const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);

		if (PlayerAction && IsValid(PlayerAction->Card))
		{
			PlayerBattleCharacter->RemoveBattleCard(PlayerAction->Card.Get());
		}

		if (EnemyAction && IsValid(EnemyAction->Card))
		{
			EnemyBattleCharacter->RemoveBattleCard(EnemyAction->Card.Get());
		}
	}
}

void UWidget_BattleMainScreen::ClearExchangeSlots() const
{
	HandWidget->ClearPlayerSelectCard();
	HandWidget->ClearEnemySelectCard();
}

void UWidget_BattleMainScreen::DisplayRoundEndUI()
{
	//국 종료 UI 표시
	FString ResultText = FString::Printf(TEXT("%d %s"), BattleManager->GetCurrentRound(), *RoundEndText);

	HandWidget->DisplayInkLine(ResultText, TurnTime);
	HandleRoundEndFinishCount += 1;
}

void UWidget_BattleMainScreen::DisplayRoundEndUIFinish()
{
	HandleRoundEndFinishCount -= 1;
	if (HandleRoundEndFinishCount <= 0)
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

	// 턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	HandleExchangeCount = 0;
	DisplayExchangeStartUI();
}

void UWidget_BattleMainScreen::DisplayExchangeStartUI()
{
	//합 시작 UI 표시

	//InkLine 호출
	HandWidget->DisplayInkLine(ExchangeStartText, TurnTime);
	HandleExchangeCount += 1;

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleRoundUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayExchangeStartUIFinish()
{
	if (HandleExchangeCount > 0)
		HandleExchangeCount--;
	if (HandleExchangeCount > 0)
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
	HandleExchangeCount = 0;
	//Pipeline UI 표시
	FString ResultText = FString::Printf(TEXT("%d %s"), ExchangeNumber, *ExchangeCountText);
	HandWidget->DisplayInkLineEnabled(ResultText, TurnTime);
}

void UWidget_BattleMainScreen::FinishExchange(int32 ExchangeIndex)
{
	if (!BattleManager || ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	HandleExchangeCount = 0;
	HandleExchangeSlot(ExchangeIndex + 1, false);
}
void UWidget_BattleMainScreen::ExchangeEnd()
{
	HandleExchangeCount = 0;
	// 턴 종료 버튼 비활성화
	HandWidget->ShowTurnEndButton(false);
	DisplayExchangeEndUI();
}

void UWidget_BattleMainScreen::DisplayExchangeEndUI()
{
	//합 종료 UI 표시

	//InkLine 표시
	HandleExchangeCount += 1;
	HandWidget->DisplayInkLine(ExchangeEndText, TurnTime);
}

void UWidget_BattleMainScreen::DisplayExchangeEndUIFinish()
{
	HandleExchangeCount -= 1;
	if (HandleExchangeCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::ExchangeEnd);
	}
}

void UWidget_BattleMainScreen::HandleExchangeEndFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CompletePhaseUI(EBattlePhase::ExchangeEnd);
}

void UWidget_BattleMainScreen::HandleExchangeSlot(int32 Index, bool bActive)
{
	HandWidget->EnableExchangeSlot(Index, bActive);
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
	UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
	ABattleCharacterBase* PlayerBattleCharacter = BattleRuntimeContext ? BattleRuntimeContext->GetPlayerCharacter() : nullptr;

	if (!PlayerBattleCharacter)
	{
		return;
	}
	// Deck data and hand widget instances have different lifetimes.
	// On the first round the deck is already populated, but the hand widget has no card instances yet.
	const bool bNeedsNewHand = !HandWidget->HasHandCards() || PlayerBattleCharacter->GetCurrentBattleCardCount() == 0;

	if (bNeedsNewHand)
	{
		PlayerBattleCharacter->RefillBattleDeckIfEmpty();
		HandWidget->DrawCards(PlayerBattleCharacter);
	}
	else
	{
		// Existing hand cards return from the lower resting position.
		HandWidget->VisibleHandCards();
	}

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
	if (!GetWorld() || !BattleManager || !BattleTargetingManager || !HandWidget
		|| ExchangeIndex != BattleManager->GetCurrentExchange() || !EnemyCard)
	{
		return;
	}

	HandWidget->PlaceEnemySelectCard(EnemyCard, ExchangeIndex);

	// Enemy targeting preview가 최소 한 프레임 렌더된 뒤 완료를 전달한다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if (BattleTargetingManager)
		{
			BattleTargetingManager->NotifyEnemyCardSelectionUIFinished();
		}
	}));
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

	const int32 ExchangeNumber = BattleManager->GetCurrentExchange() + 1;

	if (UMuksiBattleCardDataAsset* CardDataAsset = HandWidget->GetExchangeDataIndex_Player(ExchangeNumber))
	{
		BattleTargetingManager->RequestPlayerCardSelection(CardDataAsset);
	}
}

bool UWidget_BattleMainScreen::RevealEnemySelectedCard(int32 ExchangeIndex)
{
	if (!HandWidget)
	{
		return false;
	}

	return HandWidget->EnemySelectedBattleCardFlip(ExchangeIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
//------------------------------------Battle Action Sequence------------------------------------------------------------

void UWidget_BattleMainScreen::BattleActionSequenceStart()
{
	if (!HandWidget || !BattleManager)
	{
		return;
	}

	DisplayBattleActionSequenceStartUI();
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceStartUI()
{
	//합 시작 UI 표시

	//InkLine 호출
	BattleActionSequenceUIFinishCount += 1;
	HandWidget->DisplayInkLine(BattleActionSequenceStartText, TurnTime);

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleRoundUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceStartUIFinish()
{
	BattleActionSequenceUIFinishCount -= 1;
	if (BattleActionSequenceUIFinishCount <= 0)
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
	BattleActionSequenceUIFinishCount += 1;
	HandWidget->DisplayInkLine(BattleActionSequenceEndText, TurnTime);
}

void UWidget_BattleMainScreen::DisplayBattleActionSequenceEndUIFinish()
{
	BattleActionSequenceUIFinishCount -= 1;
	if (BattleActionSequenceUIFinishCount <= 0)
	{
		CompletePhaseUI(EBattlePhase::BattleActionSequenceEnd);
	}
}

//----------------------------------------------------------------------------------------------------------------------
