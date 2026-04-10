// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleManager.h"

#include "TimerManager.h"
#include "Muksi/Widgets/Battle/HandWidget.h"

#include "MuksiDebugHelper.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABattleManager::SettingBattleManager()
{
	//TODO PlayerCharacter Data
	//TODO EnemyCharacter Data
	
	//TODO Battle Start Effect
	
	if (!bHandWidgetReady)
	{
		return;
	}
	
	//일단은
	SetPhase(EBattlePhase::BattleStart);
	StartBattle();
}

void ABattleManager::BattlePipeLineExe()
{
	//테스트 용도. 더 자세한 기능은 이후에 
	
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	SettingBattleManager();
}

void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	HandWidget = nullptr;
	
	Super::EndPlay(EndPlayReason);
	
}


// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleManager::StartBattle()
{
	if (bBattleStarted)
	{
		return;
	}

	bBattleStarted = true;
	CurrentRound = 1;
	CurrentExchange = 0;

	SetPhase(EBattlePhase::BattleStart);

	Debug::Print(TEXT("Battle Start"));
	FText DisplayText = FText::FromString(TEXT("                   전투 시작!!!!                    "));
	if (!HandWidget){Debug::Print(TEXT("HandWidget is Null")); return;}
	HandWidget->ActiveInkLine(DisplayText, 3.f);
	GetWorld()->GetTimerManager().SetTimer(
		PhaseDelayTimerHandle,
		this, 
		&ABattleManager::NextPhase,
		4.f,
		false
	);
	/*GetWorld()->GetTimerManager().SetTimer(
		PhaseDelayTimerHandle,
		this,
		&ABattleManager::StartRound,
		1.0f,
		false
	);*/
}

void ABattleManager::StartRound()
{
	if (!bBattleStarted)
	{
		return;
	}
	//Debug::Print(TEXT("RoundStart Start"));
	CurrentExchange = 0;
	SetPhase(EBattlePhase::RoundStart);

	FText DisplayText = FText::FromString(FString::Printf(TEXT("                         %d 국 시작!!                      "), CurrentRound));
	Debug::Print(FString::Printf(TEXT("Round %d Start"), CurrentRound));
	
	HandWidget->ActiveInkLine(DisplayText, 3.f);
	
	GetWorld()->GetTimerManager().SetTimer(
		PhaseDelayTimerHandle,
		this,
		&ABattleManager::StartExchange,
		4.0f,
		false
	);

	//StartExchange();
}

void ABattleManager::StartExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	++CurrentExchange;
	SetPhase(EBattlePhase::ExchangeStart);

	Debug::Print(FString::Printf(TEXT("Exchange %d Start"), CurrentExchange));
	
	FText DisplayText = FText::FromString(FString::Printf(TEXT("                        %d 합 시작!!                     "), CurrentExchange));
	HandWidget->ActiveInkLine(DisplayText, 3.f);
	/*if (CurrentExchange >= 3)
	{
		StartAttack();
	}*/
	
}

void ABattleManager::StartAttack()
{
	if (!bBattleStarted)
	{
		return;
	}

	SetPhase(EBattlePhase::AttackStart);

	Debug::Print(TEXT("Attack Start"));
}

void ABattleManager::EndAttack()
{
	if (!bBattleStarted)
	{
		return;
	}

	SetPhase(EBattlePhase::AttackEnd);

	Debug::Print(TEXT("Attack End"));

	EndExchange();
}

void ABattleManager::EndExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	SetPhase(EBattlePhase::ExchangeEnd);

	Debug::Print(FString::Printf(TEXT("Exchange %d End"), CurrentExchange));

	if (CurrentExchange >= 3)
	{
		EndRound();
	}
	else
	{
		StartExchange();
	}
}

void ABattleManager::EndRound()
{
	if (!bBattleStarted)
	{
		return;
	}

	SetPhase(EBattlePhase::RoundEnd);

	Debug::Print(FString::Printf(TEXT("Round %d End"), CurrentRound));

	// 나중에 승리 조건 체크 추가
	// 지금은 테스트용으로 1국 끝나면 전투 종료
	EndBattle();
}

void ABattleManager::EndBattle()
{
	if (!bBattleStarted)
	{
		return;
	}

	SetPhase(EBattlePhase::BattleEnd);

	Debug::Print(TEXT("Battle End"));

	bBattleStarted = false;
}

void ABattleManager::AdvancePhase()
{
	switch (CurrentPhase)
	{
	case EBattlePhase::None:
		StartBattle();
		break;

	case EBattlePhase::BattleStart:
		StartRound();
		break;

	case EBattlePhase::RoundStart:
		StartExchange();
		break;

	case EBattlePhase::ExchangeStart:
		StartAttack();
		break;

	case EBattlePhase::AttackStart:
		EndAttack();
		break;

	case EBattlePhase::AttackEnd:
		EndExchange();
		break;

	case EBattlePhase::ExchangeEnd:
		if (CurrentExchange >= 3)
		{
			EndRound();
		}
		else
		{
			StartExchange();
		}
		break;

	case EBattlePhase::RoundEnd:
		EndBattle();
		break;

	case EBattlePhase::BattleEnd:
		break;

	default:
		break;
	}
}

void ABattleManager::NextPhase()
{
	switch (CurrentPhase)
	{
	case EBattlePhase::None:
		break;
	case EBattlePhase::BattleStart:
		StartRound();
		break;
	case EBattlePhase::RoundStart:
		StartRound();
		break;
	case EBattlePhase::ExchangeStart:
		StartExchange();
		break;
	case EBattlePhase::AttackStart:
		StartAttack();
		break;
	case EBattlePhase::AttackEnd:
		EndAttack();
		break;
	case EBattlePhase::ExchangeEnd:
		EndExchange();
		break;
	case EBattlePhase::RoundEnd:
		EndRound();
		break;
	
	}
}


void ABattleManager::SetPhase(EBattlePhase NewPhase)
{
	CurrentPhase = NewPhase;
}



