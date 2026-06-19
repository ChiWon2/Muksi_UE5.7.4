// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleManager.h"

#include "BattleCardPreviewComponent.h"
#include "BattleCardEffectComponent.h"
#include "CharacterData_Enemy.h"
#include "CharacterData_Player.h"
#include "TimerManager.h"
#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Character/BattleCharacter_Enemy.h"
#include "Engine/TargetPoint.h"
#include "Grid/BattleGridManager.h"
#include "Grid/BattleGridTile.h"

//공격 범위 계산 테스트
#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CardPreviewComponent = CreateDefaultSubobject<UBattleCardPreviewComponent>(
		TEXT("CardPreviewComponent")
	);
	CardEffectComponent = CreateDefaultSubobject<UBattleCardEffectComponent>(
			TEXT("CardEffectComponent")
	);
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	
	CurrentPhase = EBattlePhase::None;
	if (CardPreviewComponent)
	{
		CardPreviewComponent->InitializePreviewComponent(this, BattleGridManager);
	}
	if (CardEffectComponent)
	{
		CardEffectComponent->InitializePreviewComponent(this, BattleGridManager, CardPreviewComponent);
	}
	
	
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

	//UE_LOG(LogTemp, Log, TEXT("Battle Phase Changed: %d"), static_cast<int32>(CurrentPhase));

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
	OnRoundStarted.Broadcast();

	
	//TODO Round Start Setting
	
	
	//StartExchange();
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
	OnExchangeStarted.Broadcast();
}

void ABattleManager::RequestEndExchange()
{
	if (!bBattleStarted)
	{
		return;
	}
	if (CurrentPhase != EBattlePhase::Exchange)
	{
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

	Debug::Print(FString::Printf(TEXT("Exchange End")));

	ResolveCurrentExchange();

	//OnExchangeEnded.Broadcast(CurrentExchange);

	OnAttackStarted.Broadcast();
	
	/*if (CurrentExchange >= MaxExchangeCount)
	{
		StartAttack();
	}
	else
	{
		StartExchange();
	}*/
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
	
	OnAttack.Broadcast(CurrentAttack);

	//OnAttackStarted.Broadcast();

	// 여기서 바로 EndAttack() 하지 말 것.
	// BattleMainScreen에서 공격 UI/캐릭터 연출을 실행하고,
	// 연출이 끝나면 NotifyAttackPresentationFinished()를 호출하게 하는 구조.
}

void ABattleManager::AttackActive()
{
	//Test Using Card
	if (AttackBattleCardDataAsset)
	{
		CardEffectComponent->CardEffectUpdate(PlayerCharacterData, AttackBattleCardDataAsset);
	}
}

void ABattleManager::NotifyAttackPresentationFinished()
{
	if (!bBattleStarted)
	{
		return;
	}

	if (CurrentPhase != EBattlePhase::Attack)
	{
		/*UE_LOG(
			LogTemp,
			Warning,
			TEXT("NotifyAttackPresentationFinished ignored: CurrentPhase is not Attack")
		);*/
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

	//UE_LOG(LogTemp, Log, TEXT("Resolve Exchange %d"), CurrentExchange);
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

	//UE_LOG(LogTemp, Log, TEXT("Resolve Attack %d"), CurrentAttack);
}

void ABattleManager::UseCardByRowName(
	UCharacterDataBase* SourceCharacter,
	UCharacterDataBase* TargetCharacter,
	FName CardRowName
)
{
	if (!SourceCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UseCardByRowName: SourceCharacter is null"));
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

	//UE_LOG(LogTemp, Warning,TEXT("Setting Battle Manager is Start !!!!!!!!!!!!!!!!"));
	
	//일단은
	SetPhase(EBattlePhase::Battle);
	StartBattle();
}



//Notify Function

void ABattleManager::NotifyBattleReadyFinished()
{
	//UE_LOG(LogTemp, Log, TEXT("NotifyBattleReadyFinished called"));

	if (!bBattleStarted)
	{
		//UE_LOG(LogTemp, Warning, TEXT("NotifyBattleReadyFinished ignored: battle is not started"));
		return;
	}

	if (CurrentPhase != EBattlePhase::Ready)
	{
		//UE_LOG(LogTemp, Warning, TEXT("NotifyBattleReadyFinished ignored: CurrentPhase is not Ready"));
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

void ABattleManager::NotifyRoundReadyFinished()
{
	StartExchange();
}

void ABattleManager::NotifyRoundFinished()
{
	
}

void ABattleManager::NotifyAttackReadyFinish()
{
	OnAttack.Broadcast(1);
	StartAttack();
}


void ABattleManager::StartBattlePhase()
{
	SetPhase(EBattlePhase::Battle);

	//UE_LOG(LogTemp, Log, TEXT("Battle Phase Started"));

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
		//UE_LOG(LogTemp, Warning, TEXT("CanStartBattle failed: Player DataAsset is null"));
		return false;
	}

	if (!TestEnemyCharacterDataAsset)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CanStartBattle failed: Enemy DataAsset is null"));
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
	
	BattleGridManager->AllClearGridHovered();

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

	//UE_LOG(LogTemp, Log, TEXT("Battle Ready Started"));

	//TODO BattleCharacter Spawn
	CreateCharacter();
	
	OnBattleReady.Broadcast();
	
	
	//Test Using Card
	/*if (TestBattleCardDataAsset)
	{
		CardEffectComponent->CardEffectUpdate(PlayerCharacterData, TestBattleCardDataAsset);
	}*/
}

void ABattleManager::CreateCharacter()
{
	//나중에 CharacterDataAsset 명 교체 예정
	if (!TestPlayerCharacterDataAsset)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: TestPlayerCharacterDataAsset is null"));
		return;
	}

	if (!TestEnemyCharacterDataAsset)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: TestEnemyCharacterDataAsset is null"));
		return;
	}

	if (!PlayerSpawnPoint)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerSpawnPoint is null"));
		return;
	}

	if (!EnemySpawnPoint)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemySpawnPoint is null"));
		return;
	}
	
	
	
	
	//Spawn Function
	//나중에 변경 예정
	UWorld* World = GetWorld();
	if (!World)
		{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: World is null"));
		return;
		}
	TSubclassOf<ABattleCharacterBase> PlayerClass = TestPlayerCharacterDataAsset->BattleCharacterClass;
	
	if (!PlayerClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: Player BattleCharacterClass is null"));
		return;
	}
	
	//Spawn Enemy
	TSubclassOf<ABattleCharacterBase> EnemyClass =
		TestEnemyCharacterDataAsset->BattleCharacterClass;

	if (!EnemyClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: Enemy BattleCharacterClass is null"));
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
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerCharacterData create failed"));
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
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: PlayerBattleCharacter spawn failed"));
		return;
	}
	
	PlayerBattleCharacter->SetCharacterData(PlayerCharacterData);
	
	EnemyCharacterData = NewObject<UCharacterData_Enemy>(this);
	if (!EnemyCharacterData)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemyCharacterData create failed"));
		return;
	}
	EnemyCharacterData->InitializeFromDataAsset(TestEnemyCharacterDataAsset);
	
	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacter_Enemy>(
		EnemyClass,
		EnemySpawnPoint->GetActorTransform()
	);
	
	if (!EnemyBattleCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CreateBattleCharacters failed: EnemyBattleCharacter spawn failed"));
		return;
	}
	EnemyBattleCharacter->SetCharacterData(EnemyCharacterData);
	
	BattleGridManager->MoveCharacter(PlayerBattleCharacter, StartPlayerPoint);
	BattleGridManager->MoveCharacter(EnemyBattleCharacter, StartEnemyPoint);
	
	/*UE_LOG(
		LogTemp,
		Log,
		TEXT("CreateBattleCharacters Success / Player=%s / Enemy=%s"),
		*GetNameSafe(PlayerBattleCharacter),
		*GetNameSafe(EnemyBattleCharacter)
	);*/
}

void ABattleManager::OnHoveredGridTileChanged(ABattleGridTile* InChangeTile)
{
	if (!bIsCardTargeting)
	{
		return;
	}

	if (!CardPreviewComponent)
	{
		return;
	}
	
	if (!InChangeTile){CardPreviewComponent->ClearHoveredTile(); return;}
	if (PlayerBattleCharacter->GetCharacterPosition() == InChangeTile->GetGridCoord()){return;}
	FTransform StartTransform = BattleGridManager->GetTransformToPosition(PlayerBattleCharacter->GetCharacterPosition());
	FTransform EndTransform = BattleGridManager->GetTransformToPosition(InChangeTile->GetGridCoord());
	
	//UE_LOG(LogTemp, Log, TEXT("Player X : %d Y : %d , TargetGrid X : %d, Y : %d"), PlayerBattleCharacter->GetCharacterPosition().X, PlayerBattleCharacter->GetCharacterPosition().Y, InChangeTile->GetGridCoord().X, InChangeTile->GetGridCoord().Y);

	CardPreviewComponent->UpdateHoveredTile(StartTransform, EndTransform, BattleGridManager->CheckGridInRange(PlayerBattleCharacter->GetCharacterPosition(), InChangeTile->GetGridCoord(), CardRange));
	if (InChangeTile != SelectTargetGrid)
	{
		BattleGridManager->ClearGridHovered();
		SelectTargetGrid = InChangeTile;
		TArray<FIntPoint> TargetPoints = AttackRangeDataAsset->GetRangeCoords(BattleGridManager, SelectTargetGrid->GetGridCoord(), AttackDir);
		BattleGridManager->SetGridHovered(TargetPoints);
	}
}

void ABattleManager::TargetGridCell(ABattleGridTile* TargetGrid)
{
	UE_LOG(LogTemp, Error, TEXT("Target Grid Cell Test!! %d , %d"), TargetGrid->GetGridCoord().X, TargetGrid->GetGridCoord().Y);
	CardEffectComponent->SelectTargetGrid(TargetGrid);
}




//공격 범위 계산 테스트 용도

void ABattleManager::CalAttackRangeType(ABattleGridTile* TargetGrid)
{
	TArray<FIntPoint> TestTargetPoints = AttackRangeDataAsset->GetRangeCoords(BattleGridManager, TargetGrid->GetGridCoord(), AttackDir);
}

void ABattleManager::SetAttackDir()
{
	UE_LOG(LogTemp, Log, TEXT("R Press"));
	AttackDir += 1;
	if (AttackDir >= 6)AttackDir = 0;
}





