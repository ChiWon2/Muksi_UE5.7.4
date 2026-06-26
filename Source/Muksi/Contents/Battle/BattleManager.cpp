// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "BattleCardPreviewComponent.h"
#include "BattleCardEffectComponent.h"
#include "CharacterData_Enemy.h"
#include "CharacterData_Player.h"
#include "TimerManager.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
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
//===========================================준비(Ready)================================================================
//게임 실행 첫 프레임 이내로 끝남
void ABattleManager::ReadyStart()
{
	//현재 Phase 설정 <- 나중에 없어질 수 있음
	CurrentPhase = EBattlePhase::None;
	
	//컴포넌트 Init 설정
	ComponentInit();
	
	BattleGridManager->AllClearGridHovered();
	
	
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
	//N 국 표시 UI
	BattleMainScreen->RoundStart();
}

void ABattleManager::RoundEnd()
{
	//N국 종료 UI 표시
	
	//
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
	//1합 표시 카드 슬롯 활성화
	//1합 시작 UI 표시 <- BattleMainScreen에게 전달
	BattleMainScreen->Exchange1Start();
}

void ABattleManager::Exchange1End()
{
	//1합 카드 서로 공개<- BattleMainScreen에게 전달
	//1합 카드 이동 방향 공개
	//1합 종료 UI 표시 <- BattleMainScreen에게 전달
	BattleMainScreen->Exchange1End();
}

void ABattleManager::Exchange2Start()
{
	//2합 시작 UI 표시 <- BattleMainScreen에게 전달
	
	BattleMainScreen->Exchange2Start();
}

void ABattleManager::Exchange2End()
{
	//1합 카드 이동 방향 비공개
	
	//2합 카드 서로 공개<- BattleMainScreen에게 전달
	//2합 카드 이동 방향 공개
	//2합 종료 UI 표시 <- BattleMainScreen에게 전달
	
	BattleMainScreen->Exchange2End();
}

void ABattleManager::Exchange3Start()
{
	//3합 시작 UI 표시 <- BattleMainScreen에게 전달
	BattleMainScreen->Exchange3Start();
}

void ABattleManager::Exchange3End()
{
	//2합 카드 이동 방향 비공개
	
	//3합 카드 서로 공개<- BattleMainScreen에게 전달
	//3합 카드 이동 방향 공개
	//3합 종료 UI 표시<- BattleMainScreen에게 전달
	
	BattleMainScreen->Exchange3End();
}

void ABattleManager::ExchangeEnd()
{
	//3합 카드 이동 방향 비공개
	
	//합 종료 UI 표시<- BattleMainScreen에게 전달
	
	//합 UI 제거<- BattleMainScreen에게 전달
	
	BattleMainScreen->ExchangeEnd();
}

void ABattleManager::ExchangeCardDir(UMuksiBattleCardDataAsset* ExchangeCard)
{
	//합 도중 선택 카드 방향 정하기
	if (!CardPreviewComponent)return;
	if (!PlayerBattleCharacter)return;
	
	CardEffectComponent->CardEffectUpdate(PlayerBattleCharacter, ExchangeCard);
}

//----------------------------------------------------------------------------------------------------------------------




