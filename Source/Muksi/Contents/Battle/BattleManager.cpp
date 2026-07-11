// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "BattleCardPreviewComponent.h"
#include "BattleCardEffectComponent.h"
#include "CharacterData_Enemy.h"
#include "CharacterData_Player.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Character/BattleCharacter_Enemy.h"
#include "Engine/TargetPoint.h"
#include "Grid/BattleGridManager.h"
#include "Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

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

	
	if (!BattleMainScreen){return;}
	ReadyStart();
	
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

UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Player(int32 ExchangeCount)
{
	return PlayerSelectAction[ExchangeCount].Card;
}

UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Enemy(int32 ExchangeCount)
{
	return EnemySelectAction[ExchangeCount].Card;
}

FIntPoint ABattleManager::GetPlayerPoint() const
{
	return PlayerBattleCharacter->GetCharacterPosition();
}

FIntPoint ABattleManager::GetEnemyPoint() const
{
	return EnemyBattleCharacter->GetCharacterPosition();
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

void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
	CurrentPhase = NewPhase;
	//각 델리게이트 호출하는 형식 
	//레벨에 있는 오브젝트의 델리게이트 호출 (UI는 Widget_BattleMainScreen에서)
	
	
	//시작
	switch(CurrentPhase)
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
	switch(CurrentPhase)
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



void ABattleManager::StartBattleInternal()
{
	bBattleStarted = true;

	CurrentRound = 0;
	CurrentExchange = 0;
	CurrentAttack = 0;

	//SetPhase(EBattlePhase::Battle);

	OnBattleStarted.Broadcast();
	
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
	//나중에 전투 이벤트 개발 시 해당 Subsystem 등 에서 DataAsset을 받아오는 걸로(혹은 그 이벤트에 적용된 DataAsset)
	if (!TestPlayerCharacterDataAsset)
	{
		return;
	}

	if (!TestEnemyCharacterDataAsset)
	{
		return;
	}
	
	//Spawn Function
	//나중에 변경 예정
	UWorld* World = GetWorld();
	if (!World)
		{
		return;
		}
	TSubclassOf<ABattleCharacterBase> PlayerClass = TestPlayerCharacterDataAsset->BattleCharacterClass;
	
	if (!PlayerClass)
	{
		return;
	}
	
	//Spawn Enemy
	TSubclassOf<ABattleCharacterBase> EnemyClass =
		TestEnemyCharacterDataAsset->BattleCharacterClass;

	if (!EnemyClass)
	{
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
	
	
	
	
	//Player BattleCharacter Spawn
	PlayerBattleCharacter = World->SpawnActor<ABattleCharacter_Player>(
		PlayerClass,
		PlayerSpawnPoint->GetActorTransform()
	);
	
	if (!PlayerBattleCharacter)
	{
		return;
	}
	
	PlayerBattleCharacter->CharacterDataAsset = TestPlayerCharacterDataAsset;
	PlayerBattleCharacter->SetCharacterData(NewObject<UCharacterData_Player>(this));
	

	
	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacter_Enemy>(
		EnemyClass,
		EnemySpawnPoint->GetActorTransform()
	);
	
	if (!EnemyBattleCharacter)
	{
		return;
	}
	EnemyBattleCharacter->CharacterDataAsset = TestEnemyCharacterDataAsset;
	EnemyBattleCharacter->SetCharacterData(NewObject<UCharacterData_Enemy>(this));
	
	BattleGridManager->MoveCharacter(PlayerBattleCharacter, StartPlayerPoint);
	BattleGridManager->MoveCharacter(EnemyBattleCharacter, StartEnemyPoint);

	BattleGridManager->SetOccupied(StartPlayerPoint, PlayerBattleCharacter);
	BattleGridManager->SetOccupied(StartEnemyPoint, EnemyBattleCharacter);

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
		TargetPoints = AttackRangeDataAsset->GetRangeCoords(BattleGridManager, SelectTargetGrid->GetGridCoord(), AttackDir);
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
//===========================================준비(Ready)================================================================
//게임 실행 첫 프레임 이내로 끝남
void ABattleManager::ReadyStart()
{
	//현재 Phase 설정 <- 나중에 없어질 수 있음
	CurrentPhase = EBattlePhase::None;
	
	//컴포넌트 Init 설정
	ComponentInit();
	
	//카드 제시에서 Grid 범위 표시 비활성화
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	
	
	if (!BattleMainScreen){UE_LOG(LogTemp, Error, TEXT("Widget_BattleMainScreen is null (BattleManager.cpp)"));return;}
	
	BattleMainScreen->ReadyStart();
}

void ABattleManager::ReadyEnd()
{
	//BattleMainScreen의 ReadyEnd에서 넘어옴
	//되었는지 확인하고 Battle 단계로 넘어가기
	
	//캐릭터 DataAsset -> 나중에는 이벤트 시작 시 받아오는 걸로
	if (!TestPlayerCharacterDataAsset){UE_LOG(LogTemp, Error, TEXT("TestPlayerCharacterDataAsset is null (BattleManager)"));}
	if (!TestEnemyCharacterDataAsset){UE_LOG(LogTemp, Error, TEXT("TestEnemyCharacterDataAsset is null (BattleManager)"));}
	
	
	
	//캐릭터 스폰
	CreateCharacter();
	
	//Phase 넘기기
	BattleStart();
}

void ABattleManager::ComponentInit()
{
	if (CardPreviewComponent)
	{
		CardPreviewComponent->InitializePreviewComponent(this, BattleGridManager);
	}
	if (CardEffectComponent)
	{
		CardEffectComponent->InitializePreviewComponent(this, BattleGridManager, CardPreviewComponent);
	}
}

//==========================================전투(Battle)================================================================
void ABattleManager::BattleStart()
{
	//Current Phase 설정
	//BattleManager 델리게이트 <- 전투 시작 모션/ 기타 등등
	ChangePhase(EBattlePhase::BattleStart);
	
	
	//전투 시작 UI ex) 활협전의 한판붙자? UI 같은거
	BattleMainScreen->BattleStart();
}

void ABattleManager::BattleEnd()
{
	//Current Phase 설정
	//BattleManager 델리게이트 <- 전투 종료 모션/ 기타 등등
	ChangePhase(EBattlePhase::BattleEnd);
}
//===============================================국(Round)==============================================================
void ABattleManager::RoundStart()
{
	//BattleAction 관련 Array Empty
	SetPhase(EBattlePhase::RoundStart);
	
	AttackActions.Empty();
	PlayerSelectAction.Empty();
	EnemySelectAction.Empty();
	
	//N 국 표시 UI
	BattleMainScreen->RoundStart();
}

void ABattleManager::RoundEnd()
{
	SetPhase(EBattlePhase::RoundEnd);
	//N국 종료 UI 표시
	
	BattleMainScreen->RoundEnd();
}

//===============================================합(Exchange)===========================================================
//Battle 파이프라인 재정리 버전
//전체 흐름은 BattleManager에서 전부 기능을 마친 후 BattleMainScreen에게 전달 후 대기
//BattleMainScreen에서 기능(UI 관련)을 마친 후 BattleManager에게 다음으로 넘어가라고 통보하는 방식

						/*
						//합 시작	
	
						//1합 시작
						//1합 종료
	
						//2합 시작
						//2합 종료
	
						//3합 시작
						//3합 종료
	
						//합 종료
						*/

//합 시작
void ABattleManager::ExchangeStart()
{
	SetPhase(EBattlePhase::ExchangeStart);
	
	AttackActions.Empty();
	//핸드에 카드 배치
	
	//합 시작 UI 표시 <- BattleMainScreen에게 전달
	if (BattleMainScreen)
	{
		BattleMainScreen->ExchangeStart();
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleMainScreen is nullptr"));
	}
}

void ABattleManager::Exchange1Start()
{
	//Enemy 카드 선택
	
	//1합 표시 카드 슬롯 활성화
	//1합 시작 UI 표시 <- BattleMainScreen에게 전달
	BattleMainScreen->Exchange1Start();
}

void ABattleManager::Exchange1End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange1 End!!!!"));
	//Player 행동 선택
	SetPlayerBattleAction();
	//Enemy 행동 선택
	SetEnemyBattleAction();
	
	//1합 카드 이동 방향 공개
	SetExchangeGrid();
	SetExchangeCharacter();
	
	//1합 카드 서로 공개<- BattleMainScreen에게 전달
	//1합 종료 UI 표시 <- BattleMainScreen에게 전달
	BattleMainScreen->Exchange1End();
}

void ABattleManager::Exchange2Start()
{
	//2합 시작 UI 표시 <- BattleMainScreen에게 전달
	CurrentExchange += 1;
	BattleMainScreen->Exchange2Start();
}

void ABattleManager::Exchange2End()
{
	//1합 카드 이동 방향 비공개
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	
	
	//Player 행동 선택
	SetPlayerBattleAction();
	//Enemy 행동 선택
	SetEnemyBattleAction();
	
	
	//2합 카드 이동 방향 공개
	SetExchangeGrid(); //뭐 나중에 UI 공개랑 같이 갈 수 있음
	SetExchangeCharacter();
	
	//2합 카드 서로 공개<- BattleMainScreen에게 전달
	//2합 종료 UI 표시 <- BattleMainScreen에게 전달
	
	BattleMainScreen->Exchange2End();
}

void ABattleManager::Exchange3Start()
{
	//3합 시작 UI 표시 <- BattleMainScreen에게 전달
	CurrentExchange += 1;
	BattleMainScreen->Exchange3Start();
}

void ABattleManager::Exchange3End()
{
	//2합 카드 이동 방향 비공개
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	
	//Player 행동 선택
	SetPlayerBattleAction();
	//Enemy 행동 선택
	SetEnemyBattleAction();
	
	//3합 카드 서로 공개<- BattleMainScreen에게 전달
	
	//3합 카드 이동 방향 공개
	SetExchangeGrid();
	SetExchangeCharacter();
	//3합 종료 UI 표시<- BattleMainScreen에게 전달
	
	BattleMainScreen->Exchange3End();
}

void ABattleManager::ExchangeEnd()
{
	SetPhase(EBattlePhase::ExchangeEnd);
	//3합 카드 이동 방향 비공개
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	
	//합 종료 UI 표시<- BattleMainScreen에게 전달
	
	//합 UI 제거<- BattleMainScreen에게 전달
	
	BattleMainScreen->ExchangeEnd();
}

void ABattleManager::ExchangeN_End(int32 InIndex)
{
	switch (InIndex)
	{
	case 0:
		Exchange1End();
		break;
	case 1:
		Exchange2End();
		break;
	case 2:
		Exchange3End();
		break;
	default:
		break;
	}
}

void ABattleManager::ExchangeCardDir(UMuksiBattleCardDataAsset* ExchangeCard)
{
	//합 도중 선택 카드 방향 정하기
	if (!CardPreviewComponent)return;
	if (!PlayerBattleCharacter)return;
	AttackBattleCardDataAsset = ExchangeCard;
	CardEffectComponent->CardEffectUpdate(PlayerBattleCharacter, ExchangeCard);
}

void ABattleManager::SetPlayerBattleAction()
{
	FBattleAction BattleAction = FBattleAction();
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = AttackBattleCardDataAsset;
	BattleAction.Speed = PlayerBattleCharacter->GetCharacterSpeed() + AttackBattleCardDataAsset->CardSpeed;
	BattleAction.Attacker = PlayerBattleCharacter;
	BattleAction.bPlayerAction = true;
	
	//좌표 구하는거
	BattleAction.TargetPoints = TargetPoints;
	
	PlayerSelectAction.Add(BattleAction);
	AttackActions.Add(BattleAction);
	UE_LOG(LogTemp, Log, TEXT("SetPlayerBattleAction add %d"), PlayerSelectAction.Num());
	
}

void ABattleManager::SetEnemyBattleAction()
{
	FBattleAction BattleAction = FBattleAction();
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = EnemyBattleCharacter->GetSelectEnemyCardDataAsset(BattleGridManager, this);
	BattleAction.Attacker = EnemyBattleCharacter;
	BattleAction.bPlayerAction = false;
	
	
	//좌표 구하는거
	BattleAction.TargetPoints = EnemyBattleCharacter->GetSelectEnemyCardCoord();
	
	AttackActions.Add(BattleAction);
	EnemySelectAction.Add(BattleAction);
}

void ABattleManager::SetExchangeGrid()
{
	//일단 모든 GridTile의 인디케이터 초기화
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	
	//Action에 따른 GridTile의 ExchangeIndicator 활성화
	//Player쪽
	FBattleAction PlayerBattleAction = PlayerSelectAction[CurrentExchange];
	UE_LOG(LogTemp, Log, TEXT("Current Exchange num %d"), PlayerSelectAction.Num());
	int32 AttackType = 0;
	switch (PlayerBattleAction.Card->AttackType.AttackType)
	{
		case EMuksiBattleCardAttackType::Rush:
			AttackType = 0;
			break;
		case EMuksiBattleCardAttackType::RangeAttack:
			AttackType = 0;
			break;
		case EMuksiBattleCardAttackType::Move:
			AttackType = 1;
			break;
		case EMuksiBattleCardAttackType::Defense:
			AttackType = 2;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("AttackType is Error (BattleManager.cpp)"));
			break;
	}
	BattleGridManager->SetExchangeIndicator(AttackType, PlayerBattleAction.TargetPoints);
	UE_LOG(LogTemp, Log, TEXT("SelectCard %s"), *PlayerBattleAction.Card->CardName.ToString());
	UE_LOG(LogTemp, Log, TEXT("Player select Point {%d} , {%d}"), PlayerBattleAction.TargetPoints[0].X, PlayerBattleAction.TargetPoints[0].Y);
	
	//Enemy쪽
	FBattleAction EnemyBattleAction = EnemySelectAction[CurrentExchange];
	int32 AttackType_ = 0;
	switch (EnemyBattleAction.Card->AttackType.AttackType)
	{
	case EMuksiBattleCardAttackType::Rush:
		AttackType_ = 0;
		break;
	case EMuksiBattleCardAttackType::RangeAttack:
		AttackType_ = 0;
		break;
	case EMuksiBattleCardAttackType::Move:
		AttackType_ = 1;
		break;
	case EMuksiBattleCardAttackType::Defense:
		AttackType_ = 2;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("AttackType is Error (BattleManager.cpp)"));
		break;
	}
	UE_LOG(LogTemp, Log, TEXT("Enemy select Point {%d} , {%d}"), EnemyBattleAction.TargetPoints[0].X, EnemyBattleAction.TargetPoints[0].Y);
	BattleGridManager->SetExchangeIndicator(AttackType_, EnemyBattleAction.TargetPoints);
	
	
}

void ABattleManager::SetExchangeCharacter()
{
	//BattleGridManager->MoveActorOnGrid(PlayerBattleCharacter, PlayerBattleCharacter->GetCharacterPosition(),PlayerSelectAction[CurrentExchange].TargetPoints[0]);
	//BattleGridManager->MoveActorOnGrid(EnemyBattleCharacter, EnemyBattleCharacter->GetCharacterPosition(), EnemySelectAction[CurrentExchange].TargetPoints[0]);
}


//===============================================공격(Attack)===========================================================
//Battle 파이프라인 재정리 버전
//전체 흐름은 BattleManager에서 전부 기능을 마친 후 BattleMainScreen에게 전달 후 대기
//BattleMainScreen에서 기능(UI 관련)을 마친 후 BattleManager에게 다음으로 넘어가라고 통보하는 방식


void ABattleManager::SortAttackActions()
{
	AttackActions.Sort(
		[](const FBattleAction& A, const FBattleAction& B)
		{
			// 먼저 합 순서를 유지
			if (A.ExchangeIndex != B.ExchangeIndex)
			{
				return A.ExchangeIndex < B.ExchangeIndex;
			}

			// 같은 합에서는 속도가 높은 캐릭터가 먼저 행동
			if (A.Speed != B.Speed)
			{
				return A.Speed > B.Speed;
			}

			// 속도도 같으면 플레이어 행동을 먼저 실행
			if (A.bPlayerAction != B.bPlayerAction)
			{
				return A.bPlayerAction;
			}

			return false;
		}
	);
}

void ABattleManager::AttackStart()
{
	// 공격 단계로 변경
	SetPhase(EBattlePhase::AttackStart);

	// 이전 공격 단계에서 사용한 데이터 초기화
	//AttackActions.Empty();
	CurrentAttackActionIndex = 0;

	// 합 단계에서 선택된 카드들을 행동 배열로 변환
	

	// 실행할 행동이 없다면 바로 공격 종료 <- 있으면 안되는상황
	if (AttackActions.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AttackStart: 실행할 공격 행동이 없습니다.")
		);

		AttackEnd();
		return;
	}

	// 합 번호와 캐릭터 속도를 기준으로 행동 순서 결정
	SortAttackActions();

	// 첫 번째 행동을 가리키도록 초기화
	CurrentAttackActionIndex = 0;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("AttackStart: 총 행동 개수 %d"),
		AttackActions.Num()
	);
	//공격 시작 UI 표시
	if (BattleMainScreen)
	{
		BattleMainScreen->AttackStart();
	}
}

void ABattleManager::StartCurrentAttackAction()
{
	// 현재 인덱스에 해당하는 행동이 없으면 공격 단계 종료
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		AttackEnd();
		return;
	}

	const FBattleAction& CurrentAction =
		AttackActions[CurrentAttackActionIndex];

	// 공격자 확인
	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("StartCurrentAttackAction: Attacker is invalid. Index: %d"),
			CurrentAttackActionIndex
		);

		FinishCurrentAttackAction();
		return;
	}

	// 대상 확인
	/*if (!IsValid(CurrentAction.Target))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("StartCurrentAttackAction: Target is invalid. Index: %d"),
			CurrentAttackActionIndex
		);

		FinishCurrentAttackAction();
		return;
	}*/

	// 카드 확인
	if (!CurrentAction.Card)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("StartCurrentAttackAction: Card is invalid. Index: %d"),
			CurrentAttackActionIndex
		);

		FinishCurrentAttackAction();
		return;
	}

	/*
	 * 필요하다면 여기에서:
	 * - 공격자 사망 여부
	 * - 기절 여부
	 * - 행동 불가 상태
	 * - 대상 사망 여부
	 * 등을 확인
	 */

	/*UE_LOG(
		LogTemp,
		Log,
		TEXT("Start Action %d | Attacker: %s | Target: %s | Card: %s"),
		CurrentAttackActionIndex,
		*GetNameSafe(CurrentAction.Attacker),
		*GetNameSafe(CurrentAction.Target),
		*GetNameSafe(CurrentAction.Card)
	);*/

	// 실제 전투 데이터 처리
	ResolveCurrentAttackAction();

	// UI와 애니메이션 연출 요청
	PlayAttackAction();
}

void ABattleManager::ResolveCurrentAttackAction()
{
	// 현재 행동 인덱스 확인
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ResolveCurrentAttackAction: Invalid action index: %d"),
			CurrentAttackActionIndex
		);

		FinishCurrentAttackAction();
		return;
	}

	FBattleAction& CurrentAction =
		AttackActions[CurrentAttackActionIndex];

	// 공격자 확인
	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ResolveCurrentAttackAction: Attacker is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}

	/*// 대상 확인
	if (!IsValid(CurrentAction.Target))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ResolveCurrentAttackAction: Target is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}*/

	// 카드 확인
	if (!IsValid(CurrentAction.Card))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ResolveCurrentAttackAction: Card is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}

	// 카드 효과 컴포넌트 확인
	if (!IsValid(CardEffectComponent))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ResolveCurrentAttackAction: CardEffectComponent is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}

	/*UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Resolve Action %d | Exchange: %d | Attacker: %s | Target: %s | Card: %s"
		),
		CurrentAttackActionIndex,
		CurrentAction.ExchangeIndex + 1,
		*GetNameSafe(CurrentAction.Attacker),
		*GetNameSafe(CurrentAction.Target),
		*GetNameSafe(CurrentAction.Card)
	);*/

	/*
	 * 실제 카드 효과 적용
	 *
		나중에 BattleManager가 아닌 BattleCharacter에서 애니매이션 종료 시 호출 될 예정
	 */
	
	/*CardEffectComponent->ExecuteCardEffect(
		CurrentAction.Attacker,
		CurrentAction.Target,
		CurrentAction.Card
	);*/

	/*
	 * 여기서는 FinishCurrentAttackAction()을 호출하지 않음.
	 *
	 * 이후 PlayAttackAction()에서 연출을 실행하고,
	 * 연출이 끝나면 NotifyAttackActionFinished()를 호출한 뒤
	 * FinishCurrentAttackAction()으로 넘어감.
	 */
}

void ABattleManager::PlayAttackAction()
{
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PlayAttackAction: Invalid action index: %d"),
			CurrentAttackActionIndex
		);

		FinishCurrentAttackAction();
		return;
	}

	const FBattleAction& CurrentAction =
		AttackActions[CurrentAttackActionIndex];

	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayAttackAction: Attacker is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}

	/*if (!IsValid(CurrentAction.Target))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayAttackAction: Target is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}*/

	if (!IsValid(CurrentAction.Card))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayAttackAction: Card is invalid")
		);

		FinishCurrentAttackAction();
		return;
	}

	/*UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Play Action %d | Exchange: %d | Attacker: %s | Target: %s | Card: %s"
		),
		CurrentAttackActionIndex,
		CurrentAction.ExchangeIndex + 1,
		*GetNameSafe(CurrentAction.Attacker),
		*GetNameSafe(CurrentAction.Target),
		*GetNameSafe(CurrentAction.Card)
	);*/

	/*
	 * UI가 없다면 기다릴 연출도 없으므로
	 * 현재 행동이 끝났다고 처리
	 */
	if (!IsValid(BattleMainScreen))
	{
		NotifyAttackActionFinished();
		return;
	}

	/*
	 * UI에 현재 행동 정보를 전달해서:
	 *
	 * - 공격 카드 표시
	 * - 공격자 애니메이션 재생
	 * - 대상 피격 애니메이션 재생
	 * - 카메라 연출
	 * - 데미지 텍스트 표시
	 *
	 * 등을 처리
	 */
	/*BattleMainScreen->PlayAttackAction(
		CurrentAttackActionIndex,
		CurrentAction.Attacker,
		CurrentAction.Target,
		CurrentAction.Card
	);*/
	
	//CurrentAction.Attacker->PlayAttackAnim(CurrentAction.Card);
	if (!IsValid(BattleSequenceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSequenceManager is null"));

		NotifyAttackActionFinished();
		return;
	}

	BattleSequenceManager->OnSequenceFinished.RemoveAll(this);

	BattleSequenceManager->OnSequenceFinished.AddUObject(
		this,
		&ABattleManager::NotifyAttackActionFinished
	);

	if (!BattleSequenceManager->StartSequence(CurrentAction))
	{
		BattleSequenceManager->OnSequenceFinished.RemoveAll(this);

		NotifyAttackActionFinished();
	}
	
}

void ABattleManager::NotifyAttackActionFinished()
{
	// 현재 행동 인덱스가 유효하지 않으면 종료
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("NotifyAttackActionFinished: Invalid action index: %d"),
			CurrentAttackActionIndex
		);

		AttackEnd();
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Attack action finished. Index: %d"),
		CurrentAttackActionIndex
	);

	// 현재 행동을 마무리하고 다음 행동으로 이동
	FinishCurrentAttackAction();
}

void ABattleManager::FinishCurrentAttackAction()
{
	// 현재 행동 완료 처리
	bWaitingForAttackActionFinish = false;

	// 필요하면 여기에서 공격 후 효과 처리
	// 예:
	// - 반격
	// - 흡혈
	// - 독 데미지
	// - 버프 감소
	// - 사망 여부 확인

	++CurrentAttackActionIndex;

	// 다음 행동이 없으면 공격 단계 종료
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		AttackEnd();
		return;
	}

	// 다음 행동 시작
	StartCurrentAttackAction();
}

void ABattleManager::AttackEnd()
{
	// 공격 종료 상태로 변경
	SetPhase(EBattlePhase::AttackEnd);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("AttackEnd: All attack actions finished")
	);

	// 현재 실행 중인 행동 상태 초기화
	bWaitingForAttackActionFinish = false;
	CurrentAttackActionIndex = INDEX_NONE;

	/*
	 * 공격 행동 배열은 공격 종료 UI에서 참조할 필요가 없다면
	 * 여기서 비워도 됨.
	 */
	AttackActions.Empty();

	/*
	 * 카드 미리보기, 공격 범위, 선택 그리드 등이 남아 있다면 제거
	 */
	if (CardPreviewComponent)
	{
		CardPreviewComponent->ClearHoveredTile();
	}

	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
	}

	/*
	 * 공격 종료 UI가 있으면 UI 연출 요청.
	 * UI 연출이 끝난 뒤 NotifyAttackEndFinished()를 호출하도록 구성.
	 */
	if (BattleMainScreen)
	{
		BattleMainScreen->AttackEnd();
		return;
	}

	// UI가 없으면 바로 다음 단계 결정
	//NotifyAttackEndFinished();
}

void ABattleManager::NotifyAttackEndFinished()
{
	if (CurrentPhase != EBattlePhase::AttackEnd)
	{
		return;
	}

	/*
	 * 공격이 끝난 뒤 전투 종료 조건 확인
	 */
	if (ShouldEndBattle())
	{
		BattleEnd();
		return;
	}

	// 전투가 계속되면 현재 국 종료
	RoundEnd();
}

//----------------------------------------------------------------------------------------------------------------------




