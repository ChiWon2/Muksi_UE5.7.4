// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"


#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"


#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"


void UWidget_BattleMainScreen::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindBattleManagerEvents();
	BindHandWidgetEvents();
	EquipBattleCardArray.SetNum(3);
	if (HandWidget)HandWidget->BattleMainScreen = this;

	
	CachedBattleManager->StartBattle();
}

void UWidget_BattleMainScreen::NativeDestruct()
{
	UnbindHandWidgetEvents();
	UnbindBattleManagerEvents();
	
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

void UWidget_BattleMainScreen::BindBattleManagerEvents()
{
	CachedBattleManager = Cast<ABattleManager>(
		UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ABattleManager::StaticClass()
		)
	);

	if (!CachedBattleManager)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BattleMainScreen: BattleManager not found"));
		return;
	}

	// BattleManager 이벤트 바인딩
	// 중복 되는 경우 방지하기 위해 Remove 한 이후 바인딩

	//BattleReady 이벤트 (위젯 준비)
	CachedBattleManager->OnBattleReady.RemoveDynamic(
	this,
	&UWidget_BattleMainScreen::HandleBattleReady
);
	
	CachedBattleManager->OnBattleReady.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleBattleReady
	);
	
	//BattleStarted 이벤트
	CachedBattleManager->OnBattleStarted.RemoveDynamic(
	this,
	&UWidget_BattleMainScreen::HandleBattleStarted
);

	CachedBattleManager->OnBattleStarted.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleBattleStarted
	);
	
	
	//BattlePhase 변경시 이벤트 <- 안 쓸듯
	CachedBattleManager->OnBattlePhaseChanged.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleBattlePhaseChanged
	);

	CachedBattleManager->OnBattlePhaseChanged.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleBattlePhaseChanged
	);
	
	//국 시작 시 이벤트
	CachedBattleManager->OnRoundStarted.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleRoundStarted
	);
	
	CachedBattleManager->OnRoundStarted.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleRoundStarted
	);
	
	//합 시작 시 이벤트
	CachedBattleManager->OnExchangeStarted.RemoveDynamic(
	this,
	&UWidget_BattleMainScreen::HandleExchangedStarted
	);

	CachedBattleManager->OnExchangeStarted.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleExchangedStarted
	);
	
	//공격 시작 시 이벤트
	CachedBattleManager->OnAttackStarted.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleAttackStarted
	);
	
	CachedBattleManager->OnAttackStarted.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleAttackStarted
	);
	
	CachedBattleManager->OnAttack.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleAttackCoundData
	);
	
	CachedBattleManager->OnAttack.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleAttackCoundData
	);

	// 이벤트를 놓쳤을 수도 있으니 현재 페이즈와 UI를 즉시 동기화
	HandleBattlePhaseChanged(CachedBattleManager->GetCurrentPhase());
}

void UWidget_BattleMainScreen::UnbindBattleManagerEvents()
{
	if (!CachedBattleManager)
	{
		return;
	}
	
	//BattleStarted 시 이벤트 UnBind
	CachedBattleManager->OnBattleStarted.RemoveDynamic(
	this,
	&UWidget_BattleMainScreen::HandleBattleStarted
);

	//BattlePhase 변경 시 이벤트 UnBind <- 안 쓸듯
	CachedBattleManager->OnBattlePhaseChanged.RemoveDynamic(
		this,
		&UWidget_BattleMainScreen::HandleBattlePhaseChanged
	);

	CachedBattleManager = nullptr;
}

void UWidget_BattleMainScreen::HandleBattlePhaseChanged(EBattlePhase NewPhase)
{
	/*switch (NewPhase)
	{
	case EBattlePhase::BattleStart:
		UE_LOG(LogTemp, Log, TEXT("UI: BattleStart"));
		break;

	case EBattlePhase::PlayerTurnStart:
		UE_LOG(LogTemp, Log, TEXT("UI: PlayerTurnStart"));
		break;

	case EBattlePhase::WaitingForCard:
		UE_LOG(LogTemp, Log, TEXT("UI: 카드를 선택하세요"));
		// ShowPhaseText(TEXT("카드를 선택하세요"));
		// SetHandEnabled(true);
		// ShowTargetGuide(false);
		break;

	case EBattlePhase::WaitingForTarget:
		UE_LOG(LogTemp, Log, TEXT("UI: 대상을 선택하세요"));
		// ShowPhaseText(TEXT("대상을 선택하세요"));
		// SetHandEnabled(false);
		// ShowTargetGuide(true);
		break;

	case EBattlePhase::ResolvingCard:
		UE_LOG(LogTemp, Log, TEXT("UI: 카드 처리 중"));
		// SetHandEnabled(false);
		break;

	case EBattlePhase::EnemyTurn:
		UE_LOG(LogTemp, Log, TEXT("UI: 적 턴"));
		// ShowPhaseText(TEXT("적 턴"));
		// SetHandEnabled(false);
		// ShowTargetGuide(false);
		break;

	case EBattlePhase::BattleEnd:
		UE_LOG(LogTemp, Log, TEXT("UI: 전투 종료"));
		// PushBattleResultWidget();
		break;

	default:
		break;
	}*/
}



void UWidget_BattleMainScreen::HandleExchangedStarted_(int32 ExchangeNumber)
{
	UE_LOG(LogTemp, Error, TEXT("ExchangeNumber %d"), ExchangeNumber);
	if (ExchangeNumber == 1)
	{
		HandWidget->BuildHandFromCharacterData(PlayerBattleCharacter->GetCharacterDeck());
		/*UE_LOG(LogTemp, Log, TEXT("Exchange Test"));
		return;*/
		EnemySelectBattleCard.Empty();
		HandWidget->StartExchangeInput(ExchangeNumber);
		HandWidget->ClearEnemySelectCard();
	}
	
	if (!HandWidget)
	{
		return;
	}

	FText DisplayText = FText::FromString(
		FString::Printf(TEXT("%d 합 시작!!"), ExchangeNumber)
	);

	HandWidget->ActiveInkLine(DisplayText, 3.f);
	
	HandWidget->EnableExchangeSlots(ExchangeNumber);
	
	EnemySelectBattleCard.Add(CachedBattleManager->GetEnemyBattleCharacter()->GetSelectEnemyCardDataAsset());
	
	UE_LOG(LogTemp,Error, TEXT("Enemy Select Card is %s"), *EnemySelectBattleCard[ExchangeNumber - 1]->GetName());
	//HandWidget->ShowTurnEndButton(true);
}

void UWidget_BattleMainScreen::HandleRoundStarted()
{
	//TODO Round 시작 UI 옵션
	CachedBattleManager->NotifyRoundReadyFinished();
}

void UWidget_BattleMainScreen::HandleExchangedStarted()
{
	//합 종료까지 BattleManager와 통신 없음 
	//1합 2합 3합 전부 여기서 처리
	//TODO 턴 종료 버튼 활성화
	HandWidget->ShowTurnEndButton(true);
	
	//TODO 합 시작 UI 표시
	//CachedBattleManager->GetCurrentRound();
	
	//TODO 합 전용 카드 칸 활성화
	//UE_LOG(LogTemp, Log, TEXT("Handle ExchangedStart_ Test round %d"), CachedBattleManager->GetCurrentExchange());
	HandleExchangedStarted_(CachedBattleManager->GetCurrentExchange());
}

void UWidget_BattleMainScreen::HandleAttackStarted()
{
	//TODO Attack 관련 UI
	CachedBattleManager->NotifyAttackReadyFinish();
}

void UWidget_BattleMainScreen::HandleAttackCoundData(int32 AttackNumber)
{
	CachedBattleManager->AttackBattleCardDataAsset = EquipBattleCardArray[AttackNumber-1];
	//TODO 공격 시작 UI 시스템
	
	
	CachedBattleManager->AttackActive();
}

void UWidget_BattleMainScreen::HandleTurnEnd()
{
	if (CachedBattleManager)
	{
		if (CanRequestEndExchange())
		{
			FinishCurrentExchange();
		}
	}
	
}

void UWidget_BattleMainScreen::FinishCurrentExchange()
{
	if (!CachedBattleManager || !HandWidget)
	{
		return;
	}

	if (CachedBattleManager->GetCurrentPhase() != EBattlePhase::Exchange)
	{
		return;
	}
	
	CurrentExchange += 1;
	if (CurrentExchange >= 3)
	{
		HandWidget->ConfirmExchangeInput(CurrentExchange);
		HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[CurrentExchange - 1]);
		CachedBattleManager->RequestEndExchange();
	}else
	{
		HandWidget->ConfirmExchangeInput(CurrentExchange);
		HandWidget->PlaceEnemySelectCard(EnemySelectBattleCard[CurrentExchange - 1]);
		HandleExchangedStarted_(CurrentExchange + 1);
	}
	
	UE_LOG(LogTemp, Log, TEXT("CurrentExchange Test %d"), CurrentExchange);
	/*const int32 ExchangeNumber = CachedBattleManager->GetCurrentExchange();

	FCardEquipSlotData SlotData =
		HandWidget->GetSlotDataByExchangeNumber(ExchangeNumber);*/

	

	//
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
		&UWidget_BattleMainScreen::HandleTurnEnd
	);

	HandWidget->OnEndTurnRequested.AddDynamic(
		this,
		&UWidget_BattleMainScreen::HandleTurnEnd
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
		&UWidget_BattleMainScreen::HandleTurnEnd
	);
}

bool UWidget_BattleMainScreen::CanRequestEndExchange()
{
	if (!CachedBattleManager)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CanRequestEndExchange failed: BattleManager is null"));
		return false;
	}

	if (!HandWidget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CanRequestEndExchange failed: HandWidget is null"));
		return false;
	}

	if (!CachedBattleManager->IsBattleStarted())
	{
		//UE_LOG(LogTemp, Warning, TEXT("CanRequestEndExchange failed: battle is not started"));
		return false;
	}

	if (CachedBattleManager->GetCurrentPhase() != EBattlePhase::Exchange)
	{
		/*UE_LOG(
			LogTemp,
			Warning,
			TEXT("CanRequestEndExchange failed: CurrentPhase is not Exchange")
		);*/
		return false;
	}

	const int32 ExchangeNumber = CachedBattleManager->GetCurrentExchange();

	if (ExchangeNumber < 1 || ExchangeNumber > 3)
	{
		/*UE_LOG(
			LogTemp,
			Warning,
			TEXT("CanRequestEndExchange failed: invalid ExchangeNumber %d"),
			ExchangeNumber
		);*/
		return false;
	}

	return true;
}

void UWidget_BattleMainScreen::HandleBattleReady()
{
	//Battle Ready 시점 처리
	/*UE_LOG(LogTemp, Log, TEXT("BattleMainScreen: HandleBattleReady"));
	if (!CachedBattleManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleBattleStarted failed: CachedBattleManager is null"));
		return;
	}

	
	UMuksiCharacterDataAsset* PlayerData =
		CachedBattleManager->GetPlayerCharacterDataAsset();
		*/
	
	
	
	//return;
	//UE_LOG(LogTemp, Log, TEXT("BattleMainScreen: HandleBattleReady"));

	if (!CachedBattleManager)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HandleBattleReady failed: CachedBattleManager is null"));
		return;
	}

	if (!HandWidget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HandleBattleReady failed: HandWidget is null"));
		return;
	}

	HandWidget->InitializeExchangeSlots();
	HandWidget->ShowTurnEndButton(false);
	PlayerBattleCharacter = CachedBattleManager->GetPlayerCharacterData();
	
	//UE_LOG(LogTemp, Warning, TEXT("HandleBattleReady successfully"));
	CachedBattleManager->NotifyBattleReadyFinished();

}

void UWidget_BattleMainScreen::HandleBattleStarted()
{
	//BattleStart 시점 처리 
	UE_LOG(LogTemp, Log, TEXT("BattleMainScreen: HandleBattleStarted"));

	if (!CachedBattleManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleBattleStarted failed: CachedBattleManager is null"));
		return;
	}

	PlayerDataAsset =
		CachedBattleManager->GetPlayerCharacterDataAsset();
	
	
	if (!PlayerDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleBattleStarted failed: PlayerData is null"));
		return;
	}
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT("HandleBattleStarted PlayerData: %s"),
		*GetNameSafe(PlayerDataAsset)
	);
	
	if (!HandWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleBattleStarted failed: HandWidget is null"));
		return;
	}

	
	CachedBattleManager->NotifyBattleStartFinished();
	
}



