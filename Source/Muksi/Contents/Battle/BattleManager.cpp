// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleManager.h"

#include "CharacterData_Enemy.h"
#include "CharacterData_Player.h"
#include "TimerManager.h"
#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Engine/TargetPoint.h"

//Default Code 
/*// Sets default values
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
	SetPhase(EBattlePhase::Battle);
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

void ABattleManager::UseCardByRowName(UCharacterDataBase* SourceCharacter, UCharacterDataBase* TargetCharacter,
	FName CardRowName)
{
	if (!SourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseCardByRowName: SourceCharacter is null"));
		return;
	}

	const FMMuksiBattleCardTableRow* CardRow = SourceCharacter->FindCardRow(CardRowName);
	if (!CardRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseCardByRowName: CardRow not found - %s"), *CardRowName.ToString());
		return;
	}

	ExecuteCardEffects(*CardRow, SourceCharacter, TargetCharacter);
}

void ABattleManager::ExecuteCardEffects(const FMMuksiBattleCardTableRow& CardRow, UCharacterDataBase* SourceCharacter,
	UCharacterDataBase* TargetCharacter)
{
	/*for (const FCardEffect& Effect : CardRow.Effects)
	{
		switch (Effect.EffectType)
		{
		case ECardEffectType::Attack:
			{
				if (Effect.TargetType == ECardTargetType::Enemy)
				{
					if (TargetCharacter)
					{
						TargetCharacter->ApplyDamage(Effect.Value);

						UE_LOG(LogTemp, Log, TEXT("Card [%s] used: Attack %d"),
							*CardRow.CardName.ToString(),
							Effect.Value);
					}
				}
				break;
			}
		//TODO CardEffect Function
		case ECardEffectType::Move:
			// 나중에 구현
			break;

		case ECardEffectType::Defense:
			// 나중에 구현
			break;

		case ECardEffectType::Heal:
			// 나중에 구현
			break;

		default:
			break;
		}
	}#1#
}

void ABattleManager::StartBattle()
{
	//Set Battle Start Effect
	if (bBattleStarted)
	{
		return;
	}

	bBattleStarted = true;
	CurrentRound = 1;
	CurrentExchange = 0;

	//SetPhase(EBattlePhase::BattleStart);

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
	);#1#
	
	//Set Round Effect
	
	SetPhase(EBattlePhase::Round);
}

void ABattleManager::StartRound()
{
	//Set Start Round Effect
	if (!bBattleStarted)
	{
		return;
	}
	//Debug::Print(TEXT("RoundStart Start"));
	CurrentExchange = 0;
	//SetPhase(EBattlePhase::RoundStart);

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
	//Set Start Exchange Effect
	if (!bBattleStarted)
	{
		return;
	}

	++CurrentExchange;
	//SetPhase(EBattlePhase::ExchangeStart);

	Debug::Print(FString::Printf(TEXT("Exchange %d Start"), CurrentExchange));
	
	FText DisplayText = FText::FromString(FString::Printf(TEXT("                        %d 합 시작!!                     "), CurrentExchange));
	HandWidget->ActiveInkLine(DisplayText, 3.f);
	/*if (CurrentExchange >= 3)
	{
		StartAttack();
	}#1#
	
}

void ABattleManager::StartAttack()
{
	//Set Start Attack Effect
	if (!bBattleStarted)
	{
		return;
	}
	
	//TODO Cal Character Speed
	
	
	
	//Set First Character Attack
	
	//Set Second Character Attack

	Debug::Print(TEXT("Attack Start"));
}

void ABattleManager::EndAttack()
{
	//Set EndAttack Effect
	if (!bBattleStarted)
	{
		return;
	}

	//SetPhase(EBattlePhase::AttackEnd);

	Debug::Print(TEXT("Attack End"));

	//EndExchange();
}

void ABattleManager::EndExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	//SetPhase(EBattlePhase::ExchangeEnd);

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

	//SetPhase(EBattlePhase::RoundEnd);

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

	//SetPhase(EBattlePhase::BattleEnd);

	Debug::Print(TEXT("Battle End"));

	bBattleStarted = false;
}

void ABattleManager::AdvancePhase()
{
	/*switch (CurrentPhase)
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
	}#1#
}

void ABattleManager::NextPhase()
{
	/*switch (CurrentPhase)
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
	
	}#1#
}


void ABattleManager::SetPhase(EBattlePhase NewPhase)
{
	CurrentPhase = NewPhase;
}*/

	
ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	
	CurrentPhase = EBattlePhase::None;
}

void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABattleManager::SetPhase(EBattlePhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	CurrentPhase = NewPhase;

	UE_LOG(LogTemp, Log, TEXT("Battle Phase Changed: %d"), static_cast<int32>(CurrentPhase));

	OnBattlePhaseChanged.Broadcast(CurrentPhase);
	
}



void ABattleManager::StartRound()
{
	if (!bBattleStarted)
	{
		return;
	}

	CurrentRound++;
	CurrentExchange = 0;
	CurrentAttack = 0;

	SetPhase(EBattlePhase::Round);

	Debug::Print(FString::Printf(TEXT("Round %d Start"), CurrentRound));
	OnRoundStarted.Broadcast(CurrentRound);

	StartExchange();
}

void ABattleManager::StartExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	CurrentExchange++;

	if (CurrentExchange > MaxExchangeCount)
	{
		StartAttack();
		return;
	}

	SetPhase(EBattlePhase::Exchange);

	Debug::Print(FString::Printf(TEXT("Exchange %d Start"), CurrentExchange));
	OnExchangeStarted.Broadcast(CurrentExchange);
}

void ABattleManager::RequestEndExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	if (CurrentPhase != EBattlePhase::Exchange)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("RequestEndExchange ignored: CurrentPhase is not Exchange")
		);
		return;
	}

	EndExchange();
}

void ABattleManager::EndExchange()
{
	if (!bBattleStarted)
	{
		return;
	}

	if (CurrentPhase != EBattlePhase::Exchange)
	{
		return;
	}

	Debug::Print(FString::Printf(TEXT("Exchange %d End"), CurrentExchange));

	ResolveCurrentExchange();

	OnExchangeEnded.Broadcast(CurrentExchange);

	if (CurrentExchange >= MaxExchangeCount)
	{
		StartAttack();
	}
	else
	{
		StartExchange();
	}
}

void ABattleManager::StartAttack()
{
	if (!bBattleStarted)
	{
		return;
	}

	CurrentAttack++;

	if (CurrentAttack > MaxAttackCount)
	{
		EndRound();
		return;
	}

	SetPhase(EBattlePhase::Attack);

	Debug::Print(FString::Printf(TEXT("Attack %d Start"), CurrentAttack));
	OnAttackStarted.Broadcast(CurrentAttack);

	// 여기서 바로 EndAttack() 하지 말 것.
	// BattleMainScreen에서 공격 UI/캐릭터 연출을 실행하고,
	// 연출이 끝나면 NotifyAttackPresentationFinished()를 호출하게 하는 구조.
}

void ABattleManager::NotifyAttackPresentationFinished()
{
	if (!bBattleStarted)
	{
		return;
	}

	if (CurrentPhase != EBattlePhase::Attack)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("NotifyAttackPresentationFinished ignored: CurrentPhase is not Attack")
		);
		return;
	}

	EndAttack();
}

void ABattleManager::EndAttack()
{
	if (!bBattleStarted)
	{
		return;
	}

	if (CurrentPhase != EBattlePhase::Attack)
	{
		return;
	}

	Debug::Print(FString::Printf(TEXT("Attack %d End"), CurrentAttack));

	ResolveCurrentAttack();

	OnAttackEnded.Broadcast(CurrentAttack);

	if (CurrentAttack >= MaxAttackCount)
	{
		EndRound();
	}
	else
	{
		StartAttack();
	}
}

void ABattleManager::EndRound()
{
	if (!bBattleStarted)
	{
		return;
	}

	Debug::Print(FString::Printf(TEXT("Round %d End"), CurrentRound));

	if (ShouldEndBattle())
	{
		EndBattle();
		return;
	}

	StartRound();
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

	OnBattleEnded.Broadcast();
}

bool ABattleManager::ShouldEndBattle() const
{
	// 테스트용: 1국만 진행하고 전투 종료
	return CurrentRound >= 1;

	// 나중에는 이런 식으로 교체 가능:
	// return !PlayerCharacterData || !EnemyCharacterData || PlayerCharacterData->IsDead() || EnemyCharacterData->IsDead();
}

void ABattleManager::ResolveCurrentExchange()
{
	// TODO:
	// 현재 합 계산 처리 위치.
	// 예:
	// - 카드 선택 결과 반영
	// - 합 승패 계산
	// - 합마다 발동하는 카드 효과 처리
	// - 공격 순서 결정용 데이터 저장

	UE_LOG(LogTemp, Log, TEXT("Resolve Exchange %d"), CurrentExchange);
}

void ABattleManager::ResolveCurrentAttack()
{
	// TODO:
	// 현재 공격 계산 처리 위치.
	// 예:
	// - 공격자/대상 결정
	// - 데미지 계산
	// - 카드 효과 적용
	// - HP 감소
	// - 공격 후 효과 처리

	UE_LOG(LogTemp, Log, TEXT("Resolve Attack %d"), CurrentAttack);
}

void ABattleManager::UseCardByRowName(
	UCharacterDataBase* SourceCharacter,
	UCharacterDataBase* TargetCharacter,
	FName CardRowName
)
{
	if (!SourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseCardByRowName: SourceCharacter is null"));
		return;
	}

	/*const FMMuksiBattleCardTableRow* CardRow = SourceCharacter->FindCardRow(CardRowName);
	if (!CardRow)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UseCardByRowName: CardRow not found - %s"),
			*CardRowName.ToString()
		);
		return;
	}

	ExecuteCardEffects(*CardRow, SourceCharacter, TargetCharacter);*/
}

void ABattleManager::ExecuteCardEffects(
	const FMMuksiBattleCardTableRow& CardRow,
	UCharacterDataBase* SourceCharacter,
	UCharacterDataBase* TargetCharacter
)
{
	// TODO:
	// 기존에 주석 처리해둔 카드 효과 처리 코드를
	// 나중에 여기로 다시 복구하면 됨.
}

void ABattleManager::SettingBattleManager_()
{
	//TODO PlayerCharacter Data
	//TODO EnemyCharacter Data
	
	//TODO Battle Start Effect
	
	if (!bHandWidgetReady)
	{
		return;
	}

	UE_LOG(LogTemp, Warning,TEXT("Setting Battle Manager is Start !!!!!!!!!!!!!!!!"));
	
	//일단은
	SetPhase(EBattlePhase::Battle);
	StartBattle();
}



//Notify Function

void ABattleManager::NotifyBattleReadyFinished()
{
	UE_LOG(LogTemp, Log, TEXT("NotifyBattleReadyFinished called"));

	if (!bBattleStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyBattleReadyFinished ignored: battle is not started"));
		return;
	}

	if (CurrentPhase != EBattlePhase::Ready)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyBattleReadyFinished ignored: CurrentPhase is not Ready"));
		return;
	}

	StartBattlePhase();
}

void ABattleManager::NotifyBattleStartFinished()
{
	if (!bBattleStarted)
	{
		return;
	}
	StartRound();
}

void ABattleManager::NotifyRoundFinished()
{
	StartExchange();
}


void ABattleManager::StartBattlePhase()
{
	SetPhase(EBattlePhase::Battle);

	UE_LOG(LogTemp, Log, TEXT("Battle Phase Started"));

	OnBattleStarted.Broadcast();
}



bool ABattleManager::CanStartBattle() const
{
	if (bBattleStarted)
	{
		return false;
	}

	if (!TestPlayerCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanStartBattle failed: Player DataAsset is null"));
		return false;
	}

	if (!TestEnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanStartBattle failed: Enemy DataAsset is null"));
		return false;
	}

	return true;
}

void ABattleManager::StartBattle()
{
	/*if (bBattleStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartBattle ignored: battle already started"));
		return;
	}

	bBattleStarted = true;

	CurrentRound = 0;
	CurrentExchange = 0;
	CurrentAttack = 0;

	SetPhase(EBattlePhase::Battle);

	Debug::Print(TEXT("Battle Start"));
	OnBattleStarted.Broadcast();

	StartRound();*/
	
	if (!CanStartBattle())
	{
		return;
	}

	bBattleStarted = true;

	CurrentRound = 0;
	CurrentExchange = 0;
	CurrentAttack = 0;

	StartBattleReady();
}

void ABattleManager::StartBattleInternal()
{
	bBattleStarted = true;

	CurrentRound = 0;
	CurrentExchange = 0;
	CurrentAttack = 0;

	SetPhase(EBattlePhase::Battle);

	OnBattleStarted.Broadcast();

	StartRound();
}

void ABattleManager::StartBattleReady()
{
	SetPhase(EBattlePhase::Ready);

	UE_LOG(LogTemp, Log, TEXT("Battle Ready Started"));

	//TODO BattleCharacter Spawn
	CreateCharacter();
	
	OnBattleReady.Broadcast();
	
}

void ABattleManager::CreateCharacter()
{
	//나중에 CharacterDataAsset 명 교체 예정
	if (!TestPlayerCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: TestPlayerCharacterDataAsset is null"));
		return;
	}

	if (!TestEnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: TestEnemyCharacterDataAsset is null"));
		return;
	}

	if (!PlayerSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerSpawnPoint is null"));
		return;
	}

	if (!EnemySpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemySpawnPoint is null"));
		return;
	}
	
	
	
	
	//Spawn Function
	//나중에 변경 예정
	UWorld* World = GetWorld();
	if (!World)
		{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: World is null"));
		return;
		}
	TSubclassOf<ABattleCharacterBase> PlayerClass = TestPlayerCharacterDataAsset->BattleCharacterClass;
	
	if (!PlayerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: Player BattleCharacterClass is null"));
		return;
	}
	
	//Spawn Enemy
	TSubclassOf<ABattleCharacterBase> EnemyClass =
		TestEnemyCharacterDataAsset->BattleCharacterClass;

	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: Enemy BattleCharacterClass is null"));
		return;
	}
	
	// 기존 캐릭터 제거 <혹시 모르니>
	if (PlayerBattleCharacter)
	{
		PlayerBattleCharacter->Destroy();
		PlayerBattleCharacter = nullptr;
	}

	if (EnemyBattleCharacter)
	{
		EnemyBattleCharacter->Destroy();
		EnemyBattleCharacter = nullptr;
	}
	
	PlayerCharacterData = nullptr;
	EnemyCharacterData = nullptr;

	PlayerCharacterData = NewObject<UCharacterData_Player>(this);
	if (!PlayerCharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerCharacterData create failed"));
		return;
	}
	
	/*//Test
	if (!IsValid(TestPlayerCharacterDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("CreateBattleCharacters failed: TestPlayerCharacterDataAsset is null"));
		return;
	}

	if (!IsValid(TestEnemyCharacterDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("CreateBattleCharacters failed: TestEnemyCharacterDataAsset is null"));
		return;
	}
	//Test*/
	
	
	PlayerCharacterData->InitializeFromDataAsset(TestPlayerCharacterDataAsset);
	
	//Player BattleCharacter Spawn
	PlayerBattleCharacter = World->SpawnActor<ABattleCharacterBase>(
		PlayerClass,
		PlayerSpawnPoint->GetActorTransform()
	);
	
	if (!PlayerBattleCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerBattleCharacter spawn failed"));
		return;
	}
	
	PlayerBattleCharacter->SetCharacterData(PlayerCharacterData);
	
	EnemyCharacterData = NewObject<UCharacterData_Enemy>(this);
	if (!EnemyCharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemyCharacterData create failed"));
		return;
	}
	EnemyCharacterData->InitializeFromDataAsset(TestEnemyCharacterDataAsset);
	
	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacterBase>(
		EnemyClass,
		EnemySpawnPoint->GetActorTransform()
	);
	
	if (!EnemyBattleCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemyBattleCharacter spawn failed"));
		return;
	}
	EnemyBattleCharacter->SetCharacterData(EnemyCharacterData);
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT("CreateBattleCharacters Success / Player=%s / Enemy=%s"),
		*GetNameSafe(PlayerBattleCharacter),
		*GetNameSafe(EnemyBattleCharacter)
	);
}





