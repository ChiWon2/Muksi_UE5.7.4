// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"


#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "TimerManager.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"


void UWidget_BattleMainScreen::NativeConstruct()
{
	Super::NativeConstruct();

	BattleManager = Cast<ABattleManager>(
		UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ABattleManager::StaticClass()
		)
	);

	if (!BattleManager)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleManager is nullptr (Widget_BattleMainScreen.cpp)"));
		return;
	}

	BindBattleManagerEvents();
	BindHandWidgetEvents();

	if (HandWidget)HandWidget->BattleMainScreen = this;
	BattleManager->SetBattleMainScreen(this);
	BattleManager->ReadyStart();
}

void UWidget_BattleMainScreen::NativeDestruct()
{
	UnbindBattleManagerEvents();
	UnbindHandWidgetEvents();

	Super::NativeDestruct();
}

FReply UWidget_BattleMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	Debug::Print(TEXT("ActivatableBase MouseDown"));
	return FReply::Unhandled();
}

void UWidget_BattleMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	RequestRefreshFocus();

	/*if (AMuksiPlayerController* PC = GetOwningMuksiPlayerController())
	{
		Debug::Print(TEXT("TestNativeOnActivated"));
		//FInputModeUIOnly InputMode;
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}*/
}


void UWidget_BattleMainScreen::SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	HandWidget->SetCharacterData(Player, Enemy);
	ActivePassiveWidget->SetData(Player, Enemy);

}

void UWidget_BattleMainScreen::BindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}

	// 중복 바인딩 방지
	HandWidget->OnEndTurnRequested.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleCardSelect
	);

	HandWidget->OnEndTurnRequested.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleCardSelect
	);

	HandWidget->OnEnemyCardRevealFinished.RemoveAll(this);
	HandWidget->OnEnemyCardRevealFinished.AddUObject(
		this,
		&UWidget_BattleMainScreen::HandleEnemyCardRevealFinished
	);
}

void UWidget_BattleMainScreen::UnbindHandWidgetEvents()
{
	if (!HandWidget)
	{
		return;
	}

	HandWidget->OnEndTurnRequested.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleCardSelect
	);

	HandWidget->OnEnemyCardRevealFinished.RemoveAll(this);
}

void UWidget_BattleMainScreen::BindBattleManagerEvents()
{
	if (!BattleManager)
	{
		return;
	}
}

void UWidget_BattleMainScreen::UnbindBattleManagerEvents()
{
	if (!BattleManager)
	{
		return;
	}
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

	if (BattleManager->GetCurrentPhase() != EBattlePhase::ExchangeStart)
	{
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
	case EBattlePhase::AttackStart:
		DisplayAttackStartUIFinish();
		break;
	case EBattlePhase::AttackEnd:
		DisplayAttackEndUIFinish();
		break;
	case EBattlePhase::RoundEnd:
		DisplayRoundEndUIFinish();
		break;
	case EBattlePhase::BattleEnd:
		DisplayBattleEndUIFinish();
		break;
	}
}

void UWidget_BattleMainScreen::ReadyStart()
{
	//UI 받아올 거 설정
}

void UWidget_BattleMainScreen::ReadyEnd()
{
	//ReadyEnd 때 소환된 BattleCharacter 정보 입력
	SetCharacterData(BattleManager->GetPlayerBattleCharacter(), BattleManager->GetEnemyBattleCharacter());
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
	//일단 바로 합 시작 <- 나중 UI 연출 같은거 있으면 그 연출에 HandleBattleStartFinish()를 호출하고 거기서 BattleManager->RoundStart()하는 느낌으로
	HandleBattleUIFinishCount -= 1;

	if (HandleBattleUIFinishCount <= 0)
	{
		BattleManager->RoundStart();
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


}

void UWidget_BattleMainScreen::DisplayBattleEndUIFinish()
{
	HandleBattleUIFinishCount -= 1;
}

void UWidget_BattleMainScreen::HandleBattleStartFinish()
{
	BattleManager->RoundStart();
}

void UWidget_BattleMainScreen::HandleBattleEndFinish()
{

}

//=========================================국 시작(Round)================================================================
void UWidget_BattleMainScreen::RoundStart()
{
	HandleRoundStartFinishCount = 0;

	//RemoveCard 배열 초기화
	HandWidget->RemoveSelectedCardsData();
	//일단 다음으로 넘김
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
		BattleManager->ExchangeStart();
	}
}



void UWidget_BattleMainScreen::RoundEnd()
{
	//Exchange 단계에서 사용한 카드 묘지에 넣기
	RemoveSelectCards();
	//Exchange Slot 비우기
	ClearExchangeSlots();

	DisplayRoundEndUI();
}

void UWidget_BattleMainScreen::RemoveSelectCards()const
{
	ABattleCharacterBase* PlayerBattleCharacter = BattleManager->GetPlayerBattleCharacter();
	ABattleCharacterBase* EnemyBattleCharacter = BattleManager->GetEnemyBattleCharacter();

	if (!PlayerBattleCharacter || !EnemyBattleCharacter)
	{
		return;
	}

	for (int32 Count = 1; Count <= BattleManager->GetMaxExchangeCount(); Count++)
	{
		UMuksiBattleCardDataAsset* DataAsset = HandWidget->GetExchangeDataIndex_Player(Count);
		if (DataAsset != nullptr){UE_LOG(LogTemp, Error, TEXT("Remove Select Cards is %s"), *DataAsset->GetName());}
		else{	UE_LOG(LogTemp, Error, TEXT("Remove Select Cards is nullptr"));}
	}

	for (int32 Count = 1; Count <= BattleManager->GetMaxExchangeCount(); Count++)
	{
		PlayerBattleCharacter->RemoveBattleCard(HandWidget->GetExchangeDataIndex_Player(Count));
		EnemyBattleCharacter->RemoveBattleCard(HandWidget->GetExchangeDataIndex_Enemy(Count));


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
		BattleManager->RoundStart();
	}
}

void UWidget_BattleMainScreen::HandleRoundEndFinish()
{
	BattleManager->RoundEnd();
}

//------------------------------------------합 시작(Exchange)------------------------------------------------------------
void UWidget_BattleMainScreen::ExchangeStart()
{
	if (!HandWidget || !BattleManager)
	{
		return;
	}

	//핸드에 카드 배치
	SetBattleCardToHand();

	//턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	//임시로 만든 Vertical Box에 있는 Enemy 선택 카드 이미지 지우기
	HandWidget->ClearEnemySelectCard();

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

	BattleManager->StartExchangeSelectCard();
}

void UWidget_BattleMainScreen::StartExchangeSelectCard(int32 ExchangeIndex)
{
	if (!BattleManager || ExchangeIndex != BattleManager->GetCurrentExchange())
	{
		return;
	}

	const int32 ExchangeNumber = ExchangeIndex + 1;

	HandleExchangeSlot(ExchangeNumber, true);

	//적이 카드를 선택하는 알고리즘
	EnemySelectCardEvent();

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
	BattleManager->AdvanceExchange();
}
void UWidget_BattleMainScreen::ExchangeEnd()
{
	HandleExchangeCount = 0;
	//턴 종료 버튼 비활성화
	HandWidget->ShowTurnEndButton(false);
	//핸드에 있는 카드 패 제거
	ClearBattleCard();
	//3합 상대방 카드 그리드 표기 제거

	//합 종료 UI 표시
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
		BattleManager->AttackStart();
	}
}

void UWidget_BattleMainScreen::HandleExchangeEndFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	BattleManager->AttackStart();
}

void UWidget_BattleMainScreen::HandleExchangeSlot(int32 Index, bool bActive)
{
	HandWidget->EnableExchangeSlot(Index, bActive);
}

void UWidget_BattleMainScreen::HandleEnemyCardRevealFinished(int32 ExchangeIndex)
{
	if (!BattleManager)
	{
		return;
	}

	BattleManager->NotifyEnemyCardRevealFinished(ExchangeIndex);
}

void UWidget_BattleMainScreen::SetBattleCardToHand()
{
	ABattleCharacterBase* PlayerBattleCharacter = BattleManager->GetPlayerBattleCharacter();

	if (!PlayerBattleCharacter)
	{
		return;
	}
	//핸드에 남는 카드가 없으면 오른쪽에서 뽑기
	if (PlayerBattleCharacter->GetCurrentBattleCardCount() == 0)
	{
		HandWidget->DrawCards(PlayerBattleCharacter);
		HandWidget->HitActiveHandCards(true);
	}
	else
	{
	//핸드에 남는 카드가 있으면 아래에서 올리기
		HandWidget->VisibleHandCards();
		HandWidget->HitActiveHandCards(true);
		//HandWidget->BuildHandFromCharacter(PlayerBattleCharacter->GetCurrentBattleDeck());
	}
}

void UWidget_BattleMainScreen::ClearBattleCard()const
{
	HandWidget->InvisibleHandCards();
	HandWidget->HitActiveHandCards(false);
}

void UWidget_BattleMainScreen::EnemySelectCardEvent()
{
	if (!GetWorld() || !BattleManager)
	{
		return;
	}

	ABattleCharacterBase* EnemyBattleCharacter = BattleManager->GetEnemyBattleCharacter();

	if (!EnemyBattleCharacter)
	{
		return;
	}

	FVector2D SelectSpeed = EnemyBattleCharacter->GetCurrentSelectCardTime();
	const float RandomDelay = FMath::FRandRange(SelectSpeed[0], SelectSpeed[1]);

	GetWorld()->GetTimerManager().ClearTimer(EnemySelectCardTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		EnemySelectCardTimerHandle,
		this,
		&UWidget_BattleMainScreen::EnemyPlaceCard,
		RandomDelay,
		false
	);

	UE_LOG(LogTemp,Log,TEXT("EnemySelectCardEvent will be called after %.2f seconds"), RandomDelay);
}

void UWidget_BattleMainScreen::EnemyPlaceCard()
{
	if (ExchangeTimeOver || !BattleManager || !HandWidget)
	{
		return;
	}

	const int32 ExchangeIndex = BattleManager->GetCurrentExchange();

	if (!BattleManager->SetEnemyBattleAction())
	{
		return;
	}

	UMuksiBattleCardDataAsset* EnemyCard = BattleManager->GetBattleCardDataAssetToExchange_Enemy(ExchangeIndex);

	if (!EnemyCard)
	{
		return;
	}

	HandWidget->PlaceEnemySelectCard(EnemyCard, ExchangeIndex);
	BattleManager->NotifyEnemyCardSelectionFinished();
}

void UWidget_BattleMainScreen::HandleCardSelect()
{
	if (!CanRequestSelectCard())
	{
		return;
	}

	SelectCardDataSend();
}

bool UWidget_BattleMainScreen::CanRequestSelectCard()
{
	if (!BattleManager)
	{
		return false;
	}

	if (!HandWidget)
	{
		return false;
	}

	return BattleManager->GetCurrentPhase() == EBattlePhase::ExchangeStart
			&& BattleManager->GetCurrentExchangePhase() == EBattleExchangePhase::CardSelecting
			&& BattleManager->GetCurrentExchange() >= 0
			&& BattleManager->GetCurrentExchange() < BattleManager->GetMaxExchangeCount();
}

void UWidget_BattleMainScreen::SelectCardDataSend()const
{
	const int32 ExchangeNumber = BattleManager->GetCurrentExchange() + 1;

	if (UMuksiBattleCardDataAsset* CardDataAsset = HandWidget->GetExchangeDataIndex_Player(ExchangeNumber))
	{
		BattleManager->StartTargeting(CardDataAsset);
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
//------------------------------------------공격 시작(Attack)------------------------------------------------------------

void UWidget_BattleMainScreen::AttackStart()
{
	if (!HandWidget || !BattleManager)
	{
		return;
	}

	DisplayAttackStartUI();
}

void UWidget_BattleMainScreen::DisplayAttackStartUI()
{
	//합 시작 UI 표시

	//InkLine 호출
	HandleAttackStartFinishCount += 1;
	HandWidget->DisplayInkLine(AttackStartText, TurnTime);

	//뭐 다른 UI 표시 있으면 추가하고
	//HandleBattleStartFinishCount += 1 한 다음
	//해당 애니메이션 끝나면 거기서 DisplayBattleRoundUIFinish() 호출하기
}

void UWidget_BattleMainScreen::DisplayAttackStartUIFinish()
{
	HandleAttackStartFinishCount -= 1;
	if (HandleAttackStartFinishCount <= 0)
	{
		BattleManager->StartCurrentAttackAction();
	}
}

void UWidget_BattleMainScreen::PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter, ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset)
{

}


void UWidget_BattleMainScreen::AttackEnd()
{
	//UI 기능적으로 일단 뭐 없음
	DisplayAttackEndUI();
}

void UWidget_BattleMainScreen::DisplayAttackEndUI()
{
	HandleAttackStartFinishCount += 1;
	HandWidget->DisplayInkLine(AttackEndText, TurnTime);
}

void UWidget_BattleMainScreen::DisplayAttackEndUIFinish()
{
	HandleAttackStartFinishCount -= 1;
	if (HandleAttackStartFinishCount <= 0)
	{
		BattleManager->RoundEnd();
	}
}

//----------------------------------------------------------------------------------------------------------------------


