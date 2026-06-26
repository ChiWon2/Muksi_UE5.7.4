// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"


#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
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
	//UE_LOG(LogTemp, Warning, TEXT("Frontend Reactivated"));
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
		//UE_LOG(LogTemp, Warning, TEXT("BattleMainScreen: HandWidget is null"));
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

//메인 택스트 블록 출력 <- 나중에 새로운 메인 Text block 필요
void UWidget_BattleMainScreen::DisplayMainTextBlock(FText Text, float Time)
{
	HandWidget->ActiveInkLine(Text, Time);
}
//합 시작 텍스트 알람
void UWidget_BattleMainScreen::DisplayExchangeAlarm()
{
	
	FText ExchangeText = FText::FromString(DisplayExchangeText);
	DisplayMainTextBlock(ExchangeText, 3.f);
}
//현재 몇 합 인지 출력하는 텍스트 알람
void UWidget_BattleMainScreen::DisplayExchangeCountAlarm(int32 ExchangeCount, bool bStart)
{
	FText DisplayText;
	if (bStart)
	{
		DisplayText = FText::FromString(
		FString::Printf(TEXT("%d 합 시작!!"), ExchangeCount)
	);
	}
	else
	{
		DisplayText = FText::FromString(
		FString::Printf(TEXT("%d 합 종료!!"), ExchangeCount));
	}
	DisplayMainTextBlock(DisplayText, 3.f);
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
	
	UE_LOG(LogTemp, Error, TEXT("Ready Test"));
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
	UE_LOG(LogTemp, Log, TEXT("BattleStart (Widget_BattleMainScreen)"));
	//전투 시작 UI 표시
	DisplayMainTextBlock(FText::FromString(BattleStartText), 3.f);
	
	//일단 바로 합 시작 <- 나중 UI 연출 같은거 있으면 그 연출에 HandleBattleStartFinish()를 호출하고 거기서 CachedBattleManager->RoundStart()하는 느낌으로
	CachedBattleManager->RoundStart();
}

void UWidget_BattleMainScreen::BattleEnd()
{
	UE_LOG(LogTemp, Log, TEXT("BattleEnd (Widget_BattleMainScreen)"));
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
	UE_LOG(LogTemp, Log, TEXT("RoundStart (Widget_BattleMainScreen)"));
	RoundCound += 1;
	DisplayMainTextBlock(FText::FromString(FString::Printf(TEXT("%d %s"),RoundCound, *RoundStartText)), 3.f);
	
	//일단 다음으로 넘김
	HandleRoundStartFinish();
}

void UWidget_BattleMainScreen::HandleRoundStartFinish()
{
	CachedBattleManager->ExchangeStart();
}

void UWidget_BattleMainScreen::RoundEnd()
{
	UE_LOG(LogTemp, Log, TEXT("RoundEnd (Widget_BattleMainScreen)"));
}

void UWidget_BattleMainScreen::HandleRoundEndFinish()
{
	
}

//------------------------------------------합 시작(Exchange)------------------------------------------------------------
void UWidget_BattleMainScreen::ExchangeStart()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange Start (Widget_BattleMainScreen)"));
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
	HandWidget->BuildHandFromCharacter(PlayerBattleCharacter->GetCurrentBattleDeck());
	//합 시작 UI 표시
	DisplayExchangeAlarm();
	//턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	//임시로 만든 Vertical Box에 있는 Enemy 선택 카드 이미지 지우기
	HandWidget->ClearEnemySelectCard();
	//적 선택 카드 Array 비우기
	EnemySelectBattleCard.Empty();
	
	
	//일단 다음으로 넘기기
	HandleExchangeStartFinish();
}

void UWidget_BattleMainScreen::HandleExchangeStartFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->Exchange1Start();
}

void UWidget_BattleMainScreen::Exchange1Start()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange 1 Start (Widget_BattleMainScreen)"));
	//1합 UI 표시
	DisplayExchangeCountAlarm(1, true);
	//1합 카드 제시 칸 활성화
	HandleExchangeSlot(1, true);
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
	
}

void UWidget_BattleMainScreen::Exchange1End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange 1 End (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//1합 종료 UI 표시
	DisplayExchangeCountAlarm(1, false);
	//1합 카드 제시 칸 비활성화
	HandleExchangeSlot(1, false);
	//1합 상대방 카드 표기
}

void UWidget_BattleMainScreen::HandleExchange1EndFinish()
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
	UE_LOG(LogTemp, Log, TEXT("Exchange 2 Start (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//2합 시작 UI 표시
	DisplayExchangeCountAlarm(2, true);
	HandleExchangeSlot(2, true);
	//2합 카드 제시 칸 활성화
	
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
}

void UWidget_BattleMainScreen::Exchange2End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange 2 End (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//1합 공격 표시 그리드 효과 제거
	
	//2합 종료 UI 표시
	DisplayExchangeCountAlarm(2, false);
	//2합 카드 제시 칸 비활성화
	HandleExchangeSlot(2, false);
	//2합 상대방 카드 그리드 표기
}

void UWidget_BattleMainScreen::HandleExchange2EndFinish()
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
	UE_LOG(LogTemp, Log, TEXT("Exchange 3 Start (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//3합 시작 UI 표시
	DisplayExchangeCountAlarm(3, true);
	
	//3합 카드 제시 칸 활성화
	HandleExchangeSlot(3, true);
	//합 종료 버튼을 눌렀을 경우 해당 카드의 효과(이동/공격)방향 제시 
}

void UWidget_BattleMainScreen::Exchange3End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange 3 End (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//2합 공격 표시 효과 그리드 제거
	
	//2합 종료 UI 표시
	DisplayExchangeCountAlarm(3, false);
	//3합 카드 제시 칸 비활성화
	HandleExchangeSlot(3, false);
	//3합 상대방 카드 그리드 표기
}

void UWidget_BattleMainScreen::HandleExchange3EndFinish()
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
	UE_LOG(LogTemp, Log, TEXT("ExchangeEnd (Widget_BattleMainScreen)"));
	HandleExchangeCount = 0;
	//턴 종료 버튼 비활성화
	HandWidget->ShowTurnEndButton(false);
	//핸드에 있는 카드 패 제거
	
	//3합 상대방 카드 그리드 표기 제거
}

void UWidget_BattleMainScreen::HandleExchangeEndFinish()
{
	HandleExchangeCount--;
	if (HandleExchangeCount > 0)
	{
		return;
	}
	CachedBattleManager->RoundEnd();
}

void UWidget_BattleMainScreen::HandleExchangeSlot(int32 Index, bool bActive)
{
	HandWidget->EnableExchangeSlot(Index, bActive);
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

void UWidget_BattleMainScreen::SelectCardDataSend(int32 InIndex)
{
	CachedBattleManager->ExchangeCardDir(HandWidget->GetExchangeDataIndex(InIndex));
}

void UWidget_BattleMainScreen::FinishSelectCard()
{
	CurrentExchange += 1;
	if (CurrentExchange >= 3)
	{
		HandWidget->ConfirmExchangeInput(CurrentExchange);
		HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[CurrentExchange - 1]);
		//CachedBattleManager->RequestEndExchange();
	}else
	{
		HandWidget->ConfirmExchangeInput(CurrentExchange);
		HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[CurrentExchange - 1]);
		
		//HandleExchangedStarted_(CurrentExchange + 1);
	}
	
	UE_LOG(LogTemp, Log, TEXT("CurrentExchange Test %d"), CurrentExchange);
}

//----------------------------------------------------------------------------------------------------------------------


