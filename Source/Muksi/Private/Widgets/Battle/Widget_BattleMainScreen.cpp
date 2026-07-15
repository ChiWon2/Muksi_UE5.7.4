// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"


#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "TimerManager.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"


void UWidget_BattleMainScreen::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	//아래에 있는 것들 다 제거 예정(다 바뀔거임)
	//BindBattleManagerEvents();
	BindHandWidgetEvents();
	
	CachedBattleManager = Cast<ABattleManager>(
		UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ABattleManager::StaticClass()
		)
	);
	
	EquipBattleCardArray.SetNum(3);
	if (HandWidget)HandWidget->BattleMainScreen = this;
	CachedBattleManager->SetBattleMainScreen(this);
	CachedBattleManager->ReadyStart();
	
	
	/*CachedBattleManager->StartBattle();*/
}

void UWidget_BattleMainScreen::NativeDestruct()
{
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
}

bool UWidget_BattleMainScreen::CanRequestEndExchange()
{
	if (!CachedBattleManager)
	{
		return false;
	}

	if (!HandWidget)
	{
		return false;
	}

	if (!CachedBattleManager->IsBattleStarted())
	{
		return false;
	}



	const int32 ExchangeNumber = CachedBattleManager->GetCurrentExchange();

	if (ExchangeNumber < 1 || ExchangeNumber > 3)
	{
		return false;
	}

	return true;
}



void UWidget_BattleMainScreen::HandlePipelineUIFinish()
{
	switch (CachedBattleManager->GetCurrentPhase())
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


void UWidget_BattleMainScreen::ChangePhase(EBattlePhase NewPhase)
{
	//각 델리게이트 호출하는 형식 
	//위젯에 있는 오브젝트의 델리게이트 호출 (레벨은 BattleManager에서)
	
	//시작
	switch(NewPhase)
	{
	case EBattlePhase::None:
		break;
	case EBattlePhase::BattleStart:
		//전투 시작 이벤트
		break;
	case EBattlePhase::RoundStart:
		//국 시작 이벤트
		break;
	case EBattlePhase::ExchangeStart:
		//합 시작 이벤트
		break;
	case EBattlePhase::AttackStart: 
		//공격 시작 이벤트
		break;
	default: //있으면 안됨
		break;
	}
	
	//종료
	switch(NewPhase)
	{
	case EBattlePhase::None:
		break;
	case EBattlePhase::BattleEnd:
		//전투 종료 이벤트 <- 레벨 종료
		break;
	case EBattlePhase::RoundEnd:
		//국 시작 이벤트
		break;
	case EBattlePhase::ExchangeEnd:
		//합 시작 이벤트
		break;
	case EBattlePhase::AttackEnd: 
		//공격 시작 이벤트
		break;
	default: //있으면 안됨
		break;
	}
}

void UWidget_BattleMainScreen::ReadyStart()
{
	//UI 받아올 거 설정
	
	
	//BattleCharacter 받아오기
	PlayerBattleCharacter = CachedBattleManager->GetPlayerBattleCharacter();
	EnemyBattleCharacter = CachedBattleManager->GetEnemyBattleCharacter();
	
	ReadyEnd();
}

void UWidget_BattleMainScreen::ReadyEnd()
{
	//최종 확인 후 BattleManager에 통보
	CachedBattleManager->ReadyEnd();
}

void UWidget_BattleMainScreen::SetBattleManager()
{
	CachedBattleManager->SetBattleMainScreen(this);
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
	//일단 바로 합 시작 <- 나중 UI 연출 같은거 있으면 그 연출에 HandleBattleStartFinish()를 호출하고 거기서 CachedBattleManager->RoundStart()하는 느낌으로
	HandleBattleUIFinishCount -= 1;
	
	if (HandleBattleUIFinishCount <= 0)
	{
		CachedBattleManager->RoundStart();
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
	
	if (HandleBattleUIFinishCount <= 0)
	{
		CachedBattleManager->BattleEnd();
	}
	
}

void UWidget_BattleMainScreen::HandleBattleStartFinish()
{
	CachedBattleManager->RoundStart();
}

void UWidget_BattleMainScreen::HandleBattleEndFinish()
{
	
}

//=========================================국 시작(Round)================================================================
void UWidget_BattleMainScreen::RoundStart()
{
	RoundCound += 1;
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
	FString ResultText = FString::Printf(
		TEXT("%d %s"),
		RoundCound,
		*RoundStartText
	);
	
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
		CachedBattleManager->ExchangeStart();
	}
	//CachedBattleManager->ExchangeStart();
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
	for (int32 Count = 0; Count < 3; Count++)
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
	FString ResultText = FString::Printf(
		TEXT("%d %s"),
		RoundCound,
		*RoundEndText
	);
	
	HandWidget->DisplayInkLine(ResultText, TurnTime);
	HandleRoundEndFinishCount += 1;
}

void UWidget_BattleMainScreen::DisplayRoundEndUIFinish()
{
	HandleRoundEndFinishCount -= 1;
	if (HandleRoundEndFinishCount <= 0)
	{
		CachedBattleManager->RoundStart();
	}
}

void UWidget_BattleMainScreen::HandleRoundEndFinish()
{
	CachedBattleManager->RoundEnd();
}

//------------------------------------------합 시작(Exchange)------------------------------------------------------------
void UWidget_BattleMainScreen::ExchangeStart()
{
	//null 체크
	if (!HandWidget){return;}
	if (!PlayerBattleCharacter)
	{
		PlayerBattleCharacter = CachedBattleManager->GetPlayerBattleCharacter();
		EnemyBattleCharacter = CachedBattleManager->GetEnemyBattleCharacter();
		if (!PlayerBattleCharacter)return;
		if (!EnemyBattleCharacter)return;
	}
	
	
	//Exchange Index 값 초기화
	CurrentExchange = 1;
	//핸드에 카드 배치
	SetBattleCardToHand();
	
	//턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	//임시로 만든 Vertical Box에 있는 Enemy 선택 카드 이미지 지우기
	HandWidget->ClearEnemySelectCard();
	//적 선택 카드 Array 비우기
	EnemySelectBattleCard.Empty();
	
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
	if (HandleExchangeCount > 0)HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	
	//일단 Current Exchange 값 보고 결정
	switch (CurrentExchange)
	{
	default:
		UE_LOG(LogTemp, Error, TEXT("Current Exchange Count is Error (Widget_BattleMainScreen.cpp)"));
		break;
	case 1:
		CachedBattleManager->Exchange1Start();
		break;
	case 2:
		CachedBattleManager->Exchange2Start();
		break;
	case 3:
		CachedBattleManager->Exchange3Start();
		break;
	}
}




void UWidget_BattleMainScreen::Exchange1Start()
{
	//1합 카드 제시 칸 활성화
	HandleExchangeSlot(1, true);
	
	//플레이어가 카드를 선택했는지 체크 초기화
	PlayerSelectCardFinish = false;
	//적이 카드를 선택했는지 체크 초기화
	EnemySelectCardFinish = false;
	
	//적이 카드를 선택하는 알고리즘
	EnemySelectCardEvent();
	
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
	HandleExchangeCount = 0;
	//Pipeline UI 표시
	FString ResultText = FString::Printf(
		TEXT("%d %s"),
		CurrentExchange,
		*ExchangeCountText
	);
	HandWidget->DisplayInkLineEnabled(ResultText, TurnTime);
}

void UWidget_BattleMainScreen::Exchange1End()
{
	HandleExchangeCount = 0;
	//1합 카드 제시 칸 비활성화
	HandleExchangeSlot(1, false);

	//적이 선택한 카드 뒤집는 애니메이션
	RevealEnemySelectedCard();
	
	CurrentExchange += 1;
	//일단 넘겨 다른 곳에서 넘기는 코드를 보내야 하는데 시간 없으니까
	DisplayExchange1EndUI();
}



void UWidget_BattleMainScreen::DisplayExchange1EndUI()
{
	//일단 넘기기
	HandleExchangeCount += 1;
	DisplayExchange1EndUIFinish();
}

void UWidget_BattleMainScreen::DisplayExchange1EndUIFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->Exchange2Start();
}

void UWidget_BattleMainScreen::Exchange2Start()
{
	//플레이어가 카드를 선택했는지 체크 초기화
	PlayerSelectCardFinish = false;
	//적이 카드를 선택했는지 체크 초기화
	EnemySelectCardFinish = false;
	
	//적이 카드를 선택하는 알고리즘
	EnemySelectCardEvent();
	
	//2합 시작 UI 표시
	//Pipeline UI 표시
	FString ResultText = FString::Printf(
		TEXT("%d %s"),
		CurrentExchange,
		*ExchangeCountText
	);
	HandWidget->DisplayInkLineEnabled(ResultText, TurnTime);
	
	
	//2합 카드 제시 칸 활성화
	HandleExchangeSlot(2, true);
	
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
}

void UWidget_BattleMainScreen::Exchange2End()
{
	HandleExchangeCount = 0;
	//1합 공격 표시 그리드 효과 제거
	
	//2합 카드 제시 칸 비활성화
	HandleExchangeSlot(2, false);
	//2합 상대방 카드 표기
	RevealEnemySelectedCard();
	
	/*EnemySelectBattleCard.Add(CachedBattleManager->GetBattleCardDataAssetToExchange_Enemy(1));
	HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[1], 1);*/
	
	CurrentExchange += 1;
	//2합 종료 UI 표시
	DisplayExchange2EndUI();
}

void UWidget_BattleMainScreen::DisplayExchange2EndUI()
{
	//일단 넘겨
	DisplayExchange2EndUIFinish();
}

void UWidget_BattleMainScreen::DisplayExchange2EndUIFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->Exchange3Start();
}



void UWidget_BattleMainScreen::Exchange3Start()
{
	//플레이어가 카드를 선택했는지 체크 초기화
	PlayerSelectCardFinish = false;
	//적이 카드를 선택했는지 체크 초기화
	EnemySelectCardFinish = false;
	
	//적이 카드를 선택하는 알고리즘
	EnemySelectCardEvent();
	
	//3합 시작 UI 표시
	//Pipeline UI 표시
	FString ResultText = FString::Printf(
		TEXT("%d %s"),
		CurrentExchange,
		*ExchangeCountText
	);
	HandWidget->DisplayInkLineEnabled(ResultText, TurnTime);
	
	//3합 카드 제시 칸 활성화
	HandleExchangeSlot(3, true);
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
}

void UWidget_BattleMainScreen::Exchange3End()
{
	HandleExchangeCount = 0;
	//2합 공격 표시 효과 그리드 제거
	

	//3합 카드 제시 칸 비활성화
	HandleExchangeSlot(3, false);
	//3합 상대방 공개
	RevealEnemySelectedCard();
	
	/*//3합 상대방 카드 표기
	EnemySelectBattleCard.Add(CachedBattleManager->GetBattleCardDataAssetToExchange_Enemy(2));
	HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[2], 2);*/
	//3합 종료 UI 표시
	DisplayExchange3EndUI();
}

void UWidget_BattleMainScreen::DisplayExchange3EndUI()
{
	//일단 넘겨
	DisplayExchange3EndUIFinish();
}

void UWidget_BattleMainScreen::DisplayExchange3EndUIFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->ExchangeEnd();
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
		CachedBattleManager->AttackStart();
	}
}

void UWidget_BattleMainScreen::HandleExchangeEndFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->AttackStart();
}

void UWidget_BattleMainScreen::HandleExchangeSlot(int32 Index, bool bActive)
{
	HandWidget->EnableExchangeSlot(Index, bActive);
}

void UWidget_BattleMainScreen::SetBattleCardToHand()
{
	//핸드에 남는 카드가 없으면 오른쪽에서 뽑기
	if (PlayerBattleCharacter->GetCurrentBattleCardCount() == 0)
	{
		HandWidget->DrawCards(PlayerBattleCharacter);
		HandWidget->HitActiveHandCards(true);
	}else
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
	if (!GetWorld())
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

	UE_LOG(
		LogTemp,
		Log,
		TEXT("EnemySelectCardEvent will be called after %.2f seconds"),
		RandomDelay
	);
	
}

void UWidget_BattleMainScreen::EnemyPlaceCard()
{
	if (ExchangeTimeOver)return;
	int32 Index = CurrentExchange - 1;
	CachedBattleManager->SetEnemyBattleAction();
	EnemySelectBattleCard.Add(CachedBattleManager->GetBattleCardDataAssetToExchange_Enemy(Index));
	HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[Index], Index);
	EnemySelectCardFinish = true;
	CachedBattleManager->ExchangeN_EndReady();
}

void UWidget_BattleMainScreen::HandleCardSelect()
{
	if (CachedBattleManager)
	{
		SelectCardDataSend(CurrentExchange);
	}
}

bool UWidget_BattleMainScreen::CanRequestSelectCard()
{
	if (!CachedBattleManager)
	{
		return false;
	}

	if (!HandWidget)
	{
		return false;
	}

	return true;
}

void UWidget_BattleMainScreen::SelectCardDataSend(int32 InIndex)const
{
	if (UMuksiBattleCardDataAsset* CardDataAsset = HandWidget->GetExchangeDataIndex_Player(InIndex))
	{
		CachedBattleManager->ExchangeCardDir(CardDataAsset);
	}
}

void UWidget_BattleMainScreen::RevealEnemySelectedCard()
{
	HandWidget->EnemySelectedBattleCardFlip(CurrentExchange - 1, true);
}

//----------------------------------------------------------------------------------------------------------------------
//------------------------------------------공격 시작(Attack)------------------------------------------------------------

void UWidget_BattleMainScreen::AttackStart()
{
	//null 체크
	if (!HandWidget){return;}
	if (!PlayerBattleCharacter)
	{
		PlayerBattleCharacter = CachedBattleManager->GetPlayerBattleCharacter();
		EnemyBattleCharacter = CachedBattleManager->GetEnemyBattleCharacter();
		if (!PlayerBattleCharacter)return;
		if (!EnemyBattleCharacter)return;
	}
	
	
	//Exchange Index 값 초기화
	CurrentAttack = 1;
	/*//핸드에 카드 배치
	HandWidget->BuildHandFromCharacter(PlayerBattleCharacter->GetCurrentBattleDeck());
	//턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	//임시로 만든 Vertical Box에 있는 Enemy 선택 카드 이미지 지우기
	HandWidget->ClearEnemySelectCard();
	//적 선택 카드 Array 비우기
	EnemySelectBattleCard.Empty();*/
	
	

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
		CachedBattleManager->StartCurrentAttackAction();
	}
}

void UWidget_BattleMainScreen::PlayAttackAction(int32 InIndex, ABattleCharacterBase* AttackCharacter,
                                                ABattleCharacterBase* TargetCharacter, UMuksiBattleCardDataAsset* CardDataAsset)
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
		CachedBattleManager->RoundEnd();
	}
}

//----------------------------------------------------------------------------------------------------------------------


