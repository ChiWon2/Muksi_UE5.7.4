#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Character/BattleCharacter_Enemy.h"
#include "Character/BattleStatComponent.h"
#include "Engine/TargetPoint.h"
#include "Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Passive/CharacterPassiveComponent.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Targeting/Manager/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"



ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleManager>(this);
	}

	BattleTargetingManager = NewObject<UBattleTargetingManager>(this);

	if (!BattleMainScreen)
	{
		return;
	}

	ReadyStart();
}

void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelCurrentCardTargeting();
	BattleTargetingManager = nullptr;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->UnregisterManager<ABattleManager>(this);
	}

	Super::EndPlay(EndPlayReason);
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

FHexOffsetCoord ABattleManager::GetPlayerPoint() const
{
	return PlayerBattleCharacter->GetCharacterPosition();
}

FHexOffsetCoord ABattleManager::GetEnemyPoint() const
{
	return EnemyBattleCharacter->GetCharacterPosition();
}

void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	CurrentPhase = NewPhase;
	OnBattlePhaseChanged.Broadcast(CurrentPhase);

	switch (CurrentPhase)
	{
	case EBattlePhase::BattleStart:
		OnBattleStarted.Broadcast();
		break;

	case EBattlePhase::RoundStart:
		OnRoundStarted.Broadcast();
		break;

	case EBattlePhase::RoundEnd:
		OnRoundEnded.Broadcast();
		break;

	case EBattlePhase::ExchangeStart:
		OnExchangeStarted.Broadcast();
		break;

	case EBattlePhase::ExchangeEnd:
		OnExchangeEnded.Broadcast();
		break;

	case EBattlePhase::AttackStart:
		OnAttackStarted.Broadcast();
		break;

	case EBattlePhase::AttackEnd:
		OnAttackEnded.Broadcast();
		break;

	case EBattlePhase::BattleEnd:
		OnBattleEnded.Broadcast();
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

void ABattleManager::CreateCharacter()
{
	//나중에 전투 이벤트 개발 시 해당 Subsystem 등 에서 DataAsset을 받아오는 걸로(혹은 그 이벤트에 적용된 DataAsset)
	if (!PlayerCharacterDataAsset)
	{
		return;
	}

	if (!EnemyCharacterDataAsset)
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

	TSubclassOf<ABattleCharacterBase> PlayerClass = PlayerCharacterDataAsset->BattleCharacterClass;

	if (!PlayerClass)
	{
		return;
	}

	//Spawn Enemy
	TSubclassOf<ABattleCharacterBase> EnemyClass = EnemyCharacterDataAsset->BattleCharacterClass;

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

	checkf(IsValid(PlayerBattleCharacter), TEXT("PlayerBattleCharacter Spawn Error BattleManager"));
	if (!PlayerBattleCharacter)
	{
		return;
	}
	PlayerBattleCharacter->SetCharacterData(PlayerCharacterDataAsset, this, BattleMainScreen);

	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacter_Enemy>(EnemyClass, EnemySpawnPoint->GetActorTransform());

	if (!EnemyBattleCharacter)
	{
		return;
	}
	EnemyBattleCharacter->SetCharacterData(EnemyCharacterDataAsset, this, BattleMainScreen);
	
	BattleGridManager->PlaceCharacter(PlayerBattleCharacter, StartPlayerPoint);
	BattleGridManager->PlaceCharacter(EnemyBattleCharacter, StartEnemyPoint);

	BattleGridManager->SetOccupied(StartPlayerPoint, PlayerBattleCharacter);
	BattleGridManager->SetOccupied(StartEnemyPoint, EnemyBattleCharacter);
}

void ABattleManager::GetEnemyData()
{
	UBattleEncounterSubsystem* BattleEncounterSubsystem =
		UBattleEncounterSubsystem::Get(this);

	if (!IsValid(BattleEncounterSubsystem))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("BattleOccurSubsystem is invalid")
		);

		return;
	}

	UMuksiCharacterDataAsset* EnemyCharacterData =
		BattleEncounterSubsystem->GetCurrentEnemyData();

	if (!IsValid(EnemyCharacterData))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CurrentEnemyData is invalid")
		);

		return;
	}
	else
	{
		EnemyCharacterDataAsset = EnemyCharacterData;
		return;
	}
}


bool ABattleManager::StartCurrentCardTargeting(UMuksiBattleCardDataAsset* CardData)
{
	if (!BattleTargetingManager || !CardData)
	{
		return false;
	}

	ABattleCharacterBase* TargetingSourceCharacter = GetCurrentTargetingSourceCharacter();

	if (!TargetingSourceCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to resolve simulation targeting runtime"));
		return false;
	}

	AttackBattleCardDataAsset = CardData;

	BattleTargetingManager->StartTargeting(TargetingSourceCharacter,BattleGridManager,CardData->TargetingData);

	return BattleTargetingManager->IsTargeting();
}

bool ABattleManager::UpdateCurrentCardTargeting(const FTargetingInputContext& InputContext)
{
	if (!BattleTargetingManager)
	{
		return false;
	}

	if (!BattleTargetingManager->IsTargeting())
	{
		return false;
	}

	return BattleTargetingManager->UpdateTargeting(InputContext);
}

bool ABattleManager::ConfirmCurrentCardTargeting()
{
	if (!BattleTargetingManager)
	{
		return false;
	}

	const ETargetingConfirmResult ConfirmResult = BattleTargetingManager->ConfirmCurrentStep();

	if (ConfirmResult == ETargetingConfirmResult::Failed)
	{
		return false;
	}

	if (ConfirmResult == ETargetingConfirmResult::AdvancedToNextStep)
	{
		return false;
	}

	const FTargetingResult& ConfirmedTargetingResult = BattleTargetingManager->GetTargetingResult();

	return !ConfirmedTargetingResult.AffectedCoords.IsEmpty() || ConfirmedTargetingResult.HasSelectedCoord();
}

void ABattleManager::CancelCurrentCardTargeting()
{
	if (BattleTargetingManager)
	{
		BattleTargetingManager->CancelTargeting();
	}
}

bool ABattleManager::IsCardTargeting() const
{
	return BattleTargetingManager && BattleTargetingManager->IsTargeting();
}


//===========================================준비(Ready)================================================================
//게임 실행 첫 프레임 이내로 끝남
void ABattleManager::ReadyStart()
{
	//현재 Phase 설정 <- 나중에 없어질 수 있음
	CurrentPhase = EBattlePhase::None;
	
	//적 캐릭터 정보 얻어오기
	GetEnemyData();
	
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
	if (!PlayerCharacterDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TestPlayerCharacterDataAsset is null (BattleManager)"));
	}

	if (!EnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TestEnemyCharacterDataAsset is null (BattleManager)"));
	}

	//캐릭터 스폰
	CreateCharacter();
	BindingBattleEndEvent();

	//Phase 넘기기
	BattleMainScreen->ReadyEnd();
	BattleStart();
}



//==========================================전투(Battle)================================================================
void ABattleManager::BattleStart()
{

	if (bBattleStarted)
	{
		return;
	}

	bBattleStarted = true;
	CurrentRound = 0;
	CurrentExchange = 0;
	CurrentAttackActionIndex = INDEX_NONE;

	//Current Phase 설정
	//BattleManager 델리게이트 <- 전투 시작 모션/ 기타 등등
	ChangePhase(EBattlePhase::BattleStart);

	if (BattleMainScreen)
	{
		//전투 시작 UI ex) 활협전의 한판붙자? UI 같은거
		BattleMainScreen->BattleStart();
	}
}

void ABattleManager::BattleEnd()
{
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst3"));
	//Current Phase 설정
	//BattleManager 델리게이트 <- 전투 종료 모션/ 기타 등등 해제
	bBattleStarted = false;
	ChangePhase(EBattlePhase::BattleEnd);
	
	BattleMainScreen->BattleEnd();
	//일단 바로 넘기고 나중에 UI, 카메라 연출 같은거 있으면 그때 다시 조절하기
	//EndBattleLevel();
}

void ABattleManager::CharacterDeadPoint(ABattleCharacterBase* Character)
{
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst1"));
	//각 캐릭터별 전용 엔딩이 있으면 그걸로
	if (bIsCharacterDead){return;}
	bIsCharacterDead = true;
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst2"));
	//일단은 그냥 넘겨
	BattleEnd();
}

void ABattleManager::BindingBattleEndEvent()
{
	PlayerBattleCharacter->GetBattleStatComponent()->OnDead.AddUniqueDynamic(this, &ABattleManager::CharacterDeadPoint);
	EnemyBattleCharacter->GetBattleStatComponent()->OnDead.AddUniqueDynamic(this, &ABattleManager::CharacterDeadPoint);
}

void ABattleManager::EndBattleLevel()
{
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst4"));
	UBattleEncounterSubsystem* EncounterSubsystem = UBattleEncounterSubsystem::Get(this);
	if (!IsValid(EncounterSubsystem))
	{
		return;
	}
	
	FBattleResult BattleResult;
	//TODO 전투 종료 후 체력, 경험치(?)등등 영수증 작성하기
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst5"));
	EncounterSubsystem->FinishBattleEncounter(
	BattleResult
	);
}

//===============================================국(Round)==============================================================
//국 시작	Round 시작
void ABattleManager::RoundStart()
{
	++CurrentRound;

	ChangePhase(EBattlePhase::RoundStart);

	AttackActions.Empty();
	PlayerSelectAction.Empty();
	EnemySelectAction.Empty();
	
	BattleMainScreen->RoundStart();
}

void ABattleManager::RoundEnd()
{
	if (BattleSimulationManager)
	{
		BattleSimulationManager->OnSimulationExchangeFinished.RemoveAll(this);
		BattleSimulationManager->OnBattleSimulationFinished.RemoveAll(this);
		BattleSimulationManager->StopSimulation();
	}

	ChangePhase(EBattlePhase::RoundEnd);
	BattleMainScreen->RoundEnd();
}

//===============================================합(Exchange)===========================================================
void ABattleManager::ExchangeStart()
{
	ChangePhase(EBattlePhase::ExchangeStart);
	CurrentExchange = 0;

	AttackActions.Empty();

	if (!BattleSimulationManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSimulationManager is null"));
		return;
	}

	TArray<ABattleCharacterBase*> SourceCharacters;
	SourceCharacters.Add(PlayerBattleCharacter);
	SourceCharacters.Add(EnemyBattleCharacter);

	BattleSimulationManager->OnSimulationExchangeFinished.RemoveAll(this);
	BattleSimulationManager->OnBattleSimulationFinished.RemoveAll(this);
	BattleSimulationManager->OnSimulationExchangeFinished.AddUObject(this, &ABattleManager::HandleSimulationExchangeFinished);
	BattleSimulationManager->OnBattleSimulationFinished.AddUObject(this, &ABattleManager::HandleBattleSimulationFinished);

	if (!BattleSimulationManager->StartSimulation(BattleGridManager, SourceCharacters))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to start battle simulation"));
		return;
	}

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
	//CurrentExchange += 1;
	BattleMainScreen->Exchange1Start();
}

void ABattleManager::Exchange1End()
{
	UE_LOG(LogTemp, Log, TEXT("Exchange1 simulation start"));

	if (!StartCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to start Exchange1 simulation"));
	}
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

	if (!StartCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to start Exchange2 simulation"));
	}
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

	if (!StartCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to start Exchange3 simulation"));
	}
}

void ABattleManager::ExchangeEnd()
{
	ChangePhase(EBattlePhase::ExchangeEnd);

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

bool ABattleManager::StartCurrentExchangeSimulation()
{
	if (!BattleSimulationManager)
	{
		return false;
	}

	SetPlayerBattleAction();
	SetEnemyBattleAction();

	if (!PlayerSelectAction.IsValidIndex(CurrentExchange))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Player action is invalid. Exchange: %d"), CurrentExchange);
		return false;
	}

	if (!EnemySelectAction.IsValidIndex(CurrentExchange))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Enemy action is invalid. Exchange: %d"), CurrentExchange);
		return false;
	}

	SetExchangeGrid();

	if (!BattleSimulationManager->SetEnemyAction(EnemySelectAction[CurrentExchange]))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to set enemy simulation action. Exchange: %d"), CurrentExchange);
		return false;
	}

	if (!BattleSimulationManager->SetPlayerAction(PlayerSelectAction[CurrentExchange]))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to set player simulation action. Exchange: %d"), CurrentExchange);
		return false;
	}

	return true;
}

void ABattleManager::HandleSimulationExchangeFinished(int32 FinishedExchangeIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[BattleManager] Simulation exchange finished: %d"), FinishedExchangeIndex);

	CompleteExchangePresentation(FinishedExchangeIndex);
}

void ABattleManager::HandleBattleSimulationFinished()
{
	if (!BattleSimulationManager)
	{
		return;
	}

	AttackActions = BattleSimulationManager->GetSequenceActionQueue();

	UE_LOG(LogTemp, Log, TEXT("[BattleManager] Battle simulation finished. Sequence action count: %d"), AttackActions.Num());
}

void ABattleManager::CompleteExchangePresentation(int32 FinishedExchangeIndex)
{
	if (!BattleMainScreen)
	{
		return;
	}

	switch (FinishedExchangeIndex)
	{
	case 0:
		BattleMainScreen->Exchange1End();
		break;

	case 1:
		BattleMainScreen->Exchange2End();
		break;

	case 2:
		BattleMainScreen->Exchange3End();
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Invalid finished exchange index: %d"), FinishedExchangeIndex);
		break;
	}
}

void ABattleManager::ExchangeCardTargeting(UMuksiBattleCardDataAsset* ExchangeCard)
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
	if (!BattleTargetingManager || !PlayerBattleCharacter || !AttackBattleCardDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to create player battle action"));
		return;
	}

	FTargetingResult TargetingResult = BattleTargetingManager->GetTargetingResult();

	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		BattleSimulationManager->ConvertToSourceTargetingResult(TargetingResult);
	}

	FBattleAction BattleAction;
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = AttackBattleCardDataAsset;
	BattleAction.Speed = PlayerBattleCharacter->GetCharacterSpeed() + AttackBattleCardDataAsset->CardSpeed;
	BattleAction.Attacker = PlayerBattleCharacter;
	BattleAction.bPlayerAction = true;
	BattleAction.TargetingResult = TargetingResult;

	PlayerSelectAction.SetNum(CurrentExchange + 1);
	PlayerSelectAction[CurrentExchange] = BattleAction;

	UE_LOG(LogTemp, Log, TEXT("SetPlayerBattleAction exchange: %d"), CurrentExchange);
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
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]EnemyBattleCharacter->GetSelectEnemyCardDataAsset is null!"));
		return;
	}
	//좌표 구하는거
	BattleAction.Speed = EnemyBattleCharacter->GetCharacterSpeed() + BattleAction.Card->CardSpeed;
	BattleAction.TargetingResult.AffectedCoords = EnemyBattleCharacter->GetSelectEnemyCardCoord();

	AttackActions.Add(BattleAction);
	EnemySelectAction.Add(BattleAction);
}

void ABattleManager::SetExchangeGrid()
{
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	const FBattleAction& PlayerBattleAction = PlayerSelectAction[CurrentExchange];

	UE_LOG(LogTemp, Log, TEXT("Current Exchange num %d"), PlayerSelectAction.Num());

	TArray<FHexOffsetCoord> PlayerTargetCoords = PlayerBattleAction.TargetingResult.AffectedCoords;

	if (PlayerTargetCoords.IsEmpty() && PlayerBattleAction.TargetingResult.HasSelectedCoord())
	{
		PlayerTargetCoords.Add(PlayerBattleAction.TargetingResult.GetSelectedCoord());
	}

	BattleGridManager->SetExchangeIndicator(PlayerBattleAction.Card->AttackType.AttackType, PlayerTargetCoords);

	const FBattleAction& EnemyBattleAction = EnemySelectAction[CurrentExchange];

	int32 EnemyAttackType = 0;

	TArray<FHexOffsetCoord> EnemyTargetCoords = EnemyBattleAction.TargetingResult.AffectedCoords;

	if (EnemyTargetCoords.IsEmpty() && EnemyBattleAction.TargetingResult.HasSelectedCoord())
	{
		EnemyTargetCoords.Add(EnemyBattleAction.TargetingResult.GetSelectedCoord());
	}

	BattleGridManager->SetExchangeIndicator(EnemyBattleAction.Card->AttackType.AttackType, EnemyTargetCoords);
}



//===============================================공격(Attack)===========================================================
void ABattleManager::SortAttackActions()
{
	AttackActions.Sort([](const FBattleAction& A, const FBattleAction& B)
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
	ChangePhase(EBattlePhase::AttackStart);
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

	if (!IsValid(CurrentAction.Attacker) || !IsValid(CurrentAction.Card))
	{
		FinishCurrentAttackAction();
		return;
	}

	PlayAttackAction(CurrentAction);
}

void ABattleManager::PlayAttackAction(const FBattleAction& Action)
{
	const FBattleAction& CurrentAction = Action;

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
	ChangePhase(EBattlePhase::AttackEnd);

	UE_LOG(LogTemp, Log, TEXT("AttackEnd: All attack actions finished"));

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

ABattleCharacterBase* ABattleManager::GetCurrentTargetingSourceCharacter() const
{
	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		if (ABattleCharacterBase* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(PlayerBattleCharacter))
		{
			return SimulationCharacter;
		}
	}
	return PlayerBattleCharacter;
}

ABattleCharacterBase* ABattleManager::ResolveCurrentTargetingCharacter(ABattleCharacterBase* Character) const
{
	if (!Character)
	{
		return nullptr;
	}

	if (!BattleSimulationManager || !BattleSimulationManager->IsSimulationRunning())
	{
		return Character;
	}

	if (ABattleCharacterBase* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(Character))
	{
		return SimulationCharacter;
	}

	return Character;
}
