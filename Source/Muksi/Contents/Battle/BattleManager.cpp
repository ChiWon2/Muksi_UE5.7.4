// Fill out your copyright notice in the Description page of Project Settings.

#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "BattleCardEffectComponent.h"
#include "BattleCardPreviewComponent.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Character/BattleCharacter_Enemy.h"
#include "Engine/TargetPoint.h"
#include "Grid/BattleGridManager.h"
#include "Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Targeting/Component/MuksiCardTargetingComponent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewResolver.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CardTargetingComponent = CreateDefaultSubobject<UMuksiCardTargetingComponent>(TEXT("CardTargetingComponent"));
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	if (!BattleMainScreen)
	{
		return;
	}

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
	if (PlayerSelectAction.Num() - 1 < ExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("Exchange Count is bigger then PlayerSelectAction.Num (BattleManager.cpp)"));
		return nullptr;
	}
	UMuksiBattleCardDataAsset* Card = PlayerSelectAction[ExchangeCount].Card;
	if (!Card)
	{
		UE_LOG(LogTemp, Error, TEXT("GetBattleCardDataAssetToExchange_Enemy is Null!!!"));
		return nullptr;
	}
	
	
	return Card;
}

UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Enemy(int32 ExchangeCount)
{
	if (EnemySelectAction.Num() - 1 < ExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("Exchange Count is bigger then EnemySelectAction.Num (BattleManager.cpp)"));
		UE_LOG(LogTemp, Error, TEXT("EnemySelectAction size : %d  Exchange Count %d"), EnemySelectAction.Num(), ExchangeCount);
		return nullptr;
	}
	UMuksiBattleCardDataAsset* Card = EnemySelectAction[ExchangeCount].Card;
	if (!Card)
	{
		UE_LOG(LogTemp, Error, TEXT("GetBattleCardDataAssetToExchange_Enemy is Null!!!"));
		return nullptr;
	}
	return Card;
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
	OnBattlePhaseChanged.Broadcast(CurrentPhase);
}

void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
	CurrentPhase = NewPhase;

	//각 델리게이트 호출하는 형식
	//레벨에 있는 오브젝트의 델리게이트 호출 (UI는 Widget_BattleMainScreen에서)

	//시작
	switch (CurrentPhase)
	{
	case EBattlePhase::None:
		break;

	case EBattlePhase::BattleStart:
		break;

	case EBattlePhase::RoundStart:
		break;

	case EBattlePhase::ExchangeStart:
		break;

	case EBattlePhase::AttackStart:
		break;

	default:
		break;
	}

	//종료
	switch (CurrentPhase)
	{
	case EBattlePhase::None:
		break;

	case EBattlePhase::BattleEnd:
		break;

	case EBattlePhase::RoundEnd:
		break;

	case EBattlePhase::ExchangeEnd:
		break;

	case EBattlePhase::AttackEnd:
		break;

	default:
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
		return false;
	}

	if (!TestEnemyCharacterDataAsset)
	{
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
	OnBattleStarted.Broadcast();
}

void ABattleManager::StartBattleReady()
{
	SetPhase(EBattlePhase::Ready);

	//TODO BattleCharacter Spawn
	CreateCharacter();

	OnBattleReady.Broadcast();
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
	TSubclassOf<ABattleCharacterBase> EnemyClass = TestEnemyCharacterDataAsset->BattleCharacterClass;

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
	PlayerBattleCharacter = World->SpawnActor<ABattleCharacter_Player>(PlayerClass, PlayerSpawnPoint->GetActorTransform());

	if (!PlayerBattleCharacter)
	{
		return;
	}
	PlayerBattleCharacter->CharacterDataAsset = TestPlayerCharacterDataAsset;

	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacter_Enemy>(EnemyClass, EnemySpawnPoint->GetActorTransform());

	if (!EnemyBattleCharacter)
	{
		return;
	}
	EnemyBattleCharacter->CharacterDataAsset = TestEnemyCharacterDataAsset;
	BattleGridManager->MoveCharacter(PlayerBattleCharacter, StartPlayerPoint);
	BattleGridManager->MoveCharacter(EnemyBattleCharacter, StartEnemyPoint);

	BattleGridManager->SetOccupied(StartPlayerPoint, PlayerBattleCharacter);
	BattleGridManager->SetOccupied(StartEnemyPoint, EnemyBattleCharacter);
}

bool ABattleManager::StartCurrentCardTargeting(UMuksiBattleCardDataAsset* Card)
{
	if (!CardTargetingComponent)
	{
		return false;
	}

	if (!PlayerBattleCharacter)
	{
		return false;
	}

	if (!Card)
	{
		return false;
	}

	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->HidePreview();
	}

	CurrentTargetingResult.Reset();
	TargetPoints.Empty();

	return CardTargetingComponent->StartTargeting(PlayerBattleCharacter, Card);
}

void ABattleManager::UpdateCurrentCardTargeting(const FMuksiCardTargetingContext& Context)
{
	if (!CardTargetingComponent)
	{
		return;
	}

	if (!CardTargetingComponent->IsTargeting())
	{
		return;
	}

	CardTargetingComponent->UpdateTargeting(Context);
	CurrentTargetingResult = CardTargetingComponent->GetCurrentResult();

	if (!TargetingPreviewActor)
	{
		return;
	}

	const FMuksiTargetingPreviewCommand PreviewCommand = FMuksiTargetingPreviewResolver::BuildCommand(CardTargetingComponent->GetSourceCharacter(), CardTargetingComponent->GetCurrentCard(), CurrentTargetingResult, BattleGridManager);
	TargetingPreviewActor->UpdatePreview(PreviewCommand);
}

bool ABattleManager::ConfirmCurrentCardTargeting()
{
	if (!CardTargetingComponent)
	{
		return false;
	}

	if (!CardTargetingComponent->ConfirmTargeting(CurrentTargetingResult))
	{
		return false;
	}

	TargetPoints = CurrentTargetingResult.AffectedCoords;

	if (TargetPoints.IsEmpty() && CurrentTargetingResult.HasSelectedCoord())
	{
		TargetPoints.Add(CurrentTargetingResult.SelectedCoord);
	}

	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->HidePreview();
	}

	return !TargetPoints.IsEmpty();
}

void ABattleManager::CancelCurrentCardTargeting()
{
	if (CardTargetingComponent)
	{
		CardTargetingComponent->CancelTargeting();
	}

	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->HidePreview();
	}

	CurrentTargetingResult.Reset();
	TargetPoints.Empty();
}

bool ABattleManager::IsCardTargeting() const
{
	return CardTargetingComponent && CardTargetingComponent->IsTargeting();
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

	if (!BattleMainScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("Widget_BattleMainScreen is null (BattleManager.cpp)"));
		return;
	}

	BattleMainScreen->ReadyStart();
}

void ABattleManager::ReadyEnd()
{
	//BattleMainScreen의 ReadyEnd에서 넘어옴
	//되었는지 확인하고 Battle 단계로 넘어가기

	//캐릭터 DataAsset -> 나중에는 이벤트 시작 시 받아오는 걸로
	if (!TestPlayerCharacterDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TestPlayerCharacterDataAsset is null (BattleManager)"));
	}

	if (!TestEnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TestEnemyCharacterDataAsset is null (BattleManager)"));
	}

	//캐릭터 스폰
	CreateCharacter();

	//Phase 넘기기
	BattleStart();
}

void ABattleManager::ComponentInit()
{
	if (CardTargetingComponent)
	{
		CardTargetingComponent->InitializeTargetingComponent(BattleGridManager);
	}
	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->InitializePreviewActor(BattleGridManager);
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
//국 시작	Round 시작
void ABattleManager::RoundStart()
{
	SetPhase(EBattlePhase::RoundStart);

	AttackActions.Empty();
	PlayerSelectAction.Empty();
	EnemySelectAction.Empty();

	BattleMainScreen->RoundStart();
}

void ABattleManager::RoundEnd()
{
	SetPhase(EBattlePhase::RoundEnd);
	BattleMainScreen->RoundEnd();
}

//===============================================합(Exchange)===========================================================
void ABattleManager::ExchangeStart()
{
	SetPhase(EBattlePhase::ExchangeStart);

	AttackActions.Empty();

	if (BattleMainScreen)
	{
		BattleMainScreen->ExchangeStart();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleMainScreen is nullptr"));
	}
}

void ABattleManager::Exchange1Start()
{
	BattleMainScreen->Exchange1Start();
}

void ABattleManager::Exchange1End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange1 End!!!!"));

	SetPlayerBattleAction();
	SetEnemyBattleAction();

	SetExchangeGrid();
	SetExchangeCharacter();

	BattleMainScreen->Exchange1End();
}

void ABattleManager::Exchange2Start()
{
	CurrentExchange += 1;
	BattleMainScreen->Exchange2Start();
}

void ABattleManager::Exchange2End()
{
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	SetPlayerBattleAction();
	SetEnemyBattleAction();

	SetExchangeGrid();
	SetExchangeCharacter();

	BattleMainScreen->Exchange2End();
}

void ABattleManager::Exchange3Start()
{
	CurrentExchange += 1;
	BattleMainScreen->Exchange3Start();
}

void ABattleManager::Exchange3End()
{
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	SetPlayerBattleAction();
	SetEnemyBattleAction();

	SetExchangeGrid();
	SetExchangeCharacter();

	BattleMainScreen->Exchange3End();
}

void ABattleManager::ExchangeEnd()
{
	SetPhase(EBattlePhase::ExchangeEnd);

	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	BattleMainScreen->ExchangeEnd();
}

void ABattleManager::ExchangeN_EndReady()
{
	if (BattleMainScreen->PlayerSelectCardFinish && BattleMainScreen->EnemySelectCardFinish)
	{
		ExchangeN_End(CurrentExchange);
	}
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
	if (!ExchangeCard)
	{
		return;
	}

	if (!PlayerBattleCharacter)
	{
		return;
	}

	AttackBattleCardDataAsset = ExchangeCard;
	StartCurrentCardTargeting(ExchangeCard);
}

void ABattleManager::SetPlayerBattleAction()
{
	FBattleAction BattleAction;
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = AttackBattleCardDataAsset;
	BattleAction.Speed = PlayerBattleCharacter->GetCharacterSpeed() + AttackBattleCardDataAsset->CardSpeed;
	BattleAction.Attacker = PlayerBattleCharacter;
	BattleAction.bPlayerAction = true;
	BattleAction.TargetPoints = TargetPoints;

	PlayerSelectAction.Add(BattleAction);
	AttackActions.Add(BattleAction);

	UE_LOG(LogTemp, Log, TEXT("SetPlayerBattleAction add %d"), PlayerSelectAction.Num());
}

void ABattleManager::SetEnemyBattleAction()
{
	FBattleAction BattleAction;
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = EnemyBattleCharacter->GetSelectEnemyCardDataAsset(BattleGridManager, this);
	BattleAction.Attacker = EnemyBattleCharacter;
	BattleAction.bPlayerAction = false;
	
	if (BattleAction.Card == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyBattleCharacter->GetSelectEnemyCardDataAsset is null!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
		return;
	}
	//좌표 구하는거
	BattleAction.TargetPoints = EnemyBattleCharacter->GetSelectEnemyCardCoord();

	AttackActions.Add(BattleAction);
	EnemySelectAction.Add(BattleAction);
}

void ABattleManager::SetExchangeGrid()
{
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

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

	if (!PlayerBattleAction.TargetPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Player select Point {%d} , {%d}"), PlayerBattleAction.TargetPoints[0].X, PlayerBattleAction.TargetPoints[0].Y);
	}

	FBattleAction EnemyBattleAction = EnemySelectAction[CurrentExchange];
	int32 EnemyAttackType = 0;

	switch (EnemyBattleAction.Card->AttackType.AttackType)
	{
	case EMuksiBattleCardAttackType::Rush:
		EnemyAttackType = 0;
		break;

	case EMuksiBattleCardAttackType::RangeAttack:
		EnemyAttackType = 0;
		break;

	case EMuksiBattleCardAttackType::Move:
		EnemyAttackType = 1;
		break;

	case EMuksiBattleCardAttackType::Defense:
		EnemyAttackType = 2;
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("AttackType is Error (BattleManager.cpp)"));
		break;
	}

	if (!EnemyBattleAction.TargetPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy select Point {%d} , {%d}"), EnemyBattleAction.TargetPoints[0].X, EnemyBattleAction.TargetPoints[0].Y);
	}

	BattleGridManager->SetExchangeIndicator(EnemyAttackType, EnemyBattleAction.TargetPoints);
}

void ABattleManager::SetExchangeCharacter()
{
	//BattleGridManager->MoveActorOnGrid(PlayerBattleCharacter, PlayerBattleCharacter->GetCharacterPosition(),PlayerSelectAction[CurrentExchange].TargetPoints[0]);
	//BattleGridManager->MoveActorOnGrid(EnemyBattleCharacter, EnemyBattleCharacter->GetCharacterPosition(), EnemySelectAction[CurrentExchange].TargetPoints[0]);
}

//===============================================공격(Attack)===========================================================
void ABattleManager::SortAttackActions()
{
	AttackActions.Sort(
		[](const FBattleAction& A, const FBattleAction& B)
		{
			if (A.ExchangeIndex != B.ExchangeIndex)
			{
				return A.ExchangeIndex < B.ExchangeIndex;
			}

			if (A.Speed != B.Speed)
			{
				return A.Speed > B.Speed;
			}

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
	SetPhase(EBattlePhase::AttackStart);
	CurrentAttackActionIndex = 0;

	if (AttackActions.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackStart: 실행할 공격 행동이 없습니다."));
		AttackEnd();
		return;
	}

	SortAttackActions();
	CurrentAttackActionIndex = 0;

	UE_LOG(LogTemp, Log, TEXT("AttackStart: 총 행동 개수 %d"), AttackActions.Num());

	if (BattleMainScreen)
	{
		BattleMainScreen->AttackStart();
	}
}

void ABattleManager::StartCurrentAttackAction()
{
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		AttackEnd();
		return;
	}

	const FBattleAction& CurrentAction = AttackActions[CurrentAttackActionIndex];

	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartCurrentAttackAction: Attacker is invalid. Index: %d"), CurrentAttackActionIndex);
		FinishCurrentAttackAction();
		return;
	}

	if (!CurrentAction.Card)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartCurrentAttackAction: Card is invalid. Index: %d"), CurrentAttackActionIndex);
		FinishCurrentAttackAction();
		return;
	}

	ResolveCurrentAttackAction();
	PlayAttackAction();
}

void ABattleManager::ResolveCurrentAttackAction()
{
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("ResolveCurrentAttackAction: Invalid action index: %d"), CurrentAttackActionIndex);
		FinishCurrentAttackAction();
		return;
	}

	FBattleAction& CurrentAction = AttackActions[CurrentAttackActionIndex];

	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResolveCurrentAttackAction: Attacker is invalid"));
		FinishCurrentAttackAction();
		return;
	}

	if (!IsValid(CurrentAction.Card))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResolveCurrentAttackAction: Card is invalid"));
		FinishCurrentAttackAction();
		return;
	}
}

void ABattleManager::PlayAttackAction()
{
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayAttackAction: Invalid action index: %d"), CurrentAttackActionIndex);
		FinishCurrentAttackAction();
		return;
	}

	const FBattleAction& CurrentAction = AttackActions[CurrentAttackActionIndex];

	if (!IsValid(CurrentAction.Attacker))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAttackAction: Attacker is invalid"));
		FinishCurrentAttackAction();
		return;
	}

	if (!IsValid(CurrentAction.Card))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAttackAction: Card is invalid"));
		FinishCurrentAttackAction();
		return;
	}

	if (!IsValid(BattleMainScreen))
	{
		NotifyAttackActionFinished();
		return;
	}

	if (!IsValid(BattleSequenceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSequenceManager is null"));
		NotifyAttackActionFinished();
		return;
	}

	BattleSequenceManager->OnSequenceFinished.RemoveAll(this);
	BattleSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleManager::NotifyAttackActionFinished);

	if (!BattleSequenceManager->StartSequence(CurrentAction))
	{
		BattleSequenceManager->OnSequenceFinished.RemoveAll(this);
		NotifyAttackActionFinished();
	}
}

void ABattleManager::NotifyAttackActionFinished()
{
	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyAttackActionFinished: Invalid action index: %d"), CurrentAttackActionIndex);
		AttackEnd();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Attack action finished. Index: %d"), CurrentAttackActionIndex);

	FinishCurrentAttackAction();
}

void ABattleManager::FinishCurrentAttackAction()
{
	bWaitingForAttackActionFinish = false;
	++CurrentAttackActionIndex;

	if (!AttackActions.IsValidIndex(CurrentAttackActionIndex))
	{
		AttackEnd();
		return;
	}

	StartCurrentAttackAction();
}

void ABattleManager::AttackEnd()
{
	SetPhase(EBattlePhase::AttackEnd);

	UE_LOG(LogTemp, Log, TEXT("AttackEnd: All attack actions finished"));

	bWaitingForAttackActionFinish = false;
	CurrentAttackActionIndex = INDEX_NONE;
	AttackActions.Empty();

	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
	}

	if (BattleMainScreen)
	{
		BattleMainScreen->AttackEnd();
		return;
	}
}

void ABattleManager::NotifyAttackEndFinished()
{
	if (CurrentPhase != EBattlePhase::AttackEnd)
	{
		return;
	}

	if (ShouldEndBattle())
	{
		BattleEnd();
		return;
	}

	RoundEnd();
}