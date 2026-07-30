#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "Character/BattleCharacter_Enemy.h"
#include "Grid/BattleGridManager.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Targeting/Manager/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"


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

	const EBattlePhase OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	OnBattlePhaseChanged.Broadcast(OldPhase, CurrentPhase);
}

void ABattleManager::SetCurrentExchangeFlowState(EBattleExchangeFlowState NewState)
{
	if (CurrentExchangeFlowState == NewState)
	{
		return;
	}

	CurrentExchangeFlowState = NewState;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[BattleManager] Exchange %d flow state changed: %d"),
		CurrentExchange,
		static_cast<int32>(CurrentExchangeFlowState)
	);
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
	if (!TestPlayerCharacterDataAsset || !TestEnemyCharacterDataAsset)
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
	PlayerBattleCharacter = World->SpawnActor<ABattleCharacter_Player>(PlayerClass, GetActorTransform());

	if (!PlayerBattleCharacter)
	{
		return;
	}
	PlayerBattleCharacter->SetCharacterData(TestPlayerCharacterDataAsset, this, BattleMainScreen);

	//Enemy BattleCharacter Spawn
	EnemyBattleCharacter = World->SpawnActor<ABattleCharacter_Enemy>(EnemyClass, GetActorTransform());

	if (!EnemyBattleCharacter)
	{
		return;
	}
	EnemyBattleCharacter->SetCharacterData(TestEnemyCharacterDataAsset, this, BattleMainScreen);

	BattleGridManager->PlaceCharacter(PlayerBattleCharacter, StartPlayerPoint);
	BattleGridManager->PlaceCharacter(EnemyBattleCharacter, StartEnemyPoint);
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

	BattleTargetingManager->StartTargeting(TargetingSourceCharacter, BattleGridManager, CardData->TargetingData);

	return BattleTargetingManager->IsTargeting();
}

bool ABattleManager::UpdateCurrentCardTargeting(const FTargetingInputContext& InputContext)
{
	if (!BattleTargetingManager || !BattleTargetingManager->IsTargeting())
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

	const bool bTargetingConfirmed = !ConfirmedTargetingResult.AffectedCoords.IsEmpty() || ConfirmedTargetingResult.HasSelectedCoord();

	if (bTargetingConfirmed)
	{
		NotifyPlayerCardSelectionFinished();
	}

	return bTargetingConfirmed;
}

void ABattleManager::CancelCurrentCardTargeting()
{
	if (BattleTargetingManager)
	{
		BattleTargetingManager->CancelTargeting();
	}

	if (CurrentPhase == EBattlePhase::ExchangeStart &&
		CurrentExchangeFlowState == EBattleExchangeFlowState::Targeting &&
		!bPlayerCardSelectionFinished)
	{
		SetCurrentExchangeFlowState(EBattleExchangeFlowState::CardSelecting);
	}
}

bool ABattleManager::IsCardTargeting() const
{
	return BattleTargetingManager && BattleTargetingManager->IsTargeting();
}


//===========================================준비(Ready)================================================================
// ======== ReadyStart() -> BattleMainScreen::ReadyStart() -> ReadyEnd() -> BattleMainScreen:: ReadyEnd()================
//게임 실행 첫 프레임 이내로 끝남
void ABattleManager::ReadyStart()
{
	//현재 Phase 설정 <- 나중에 없어질 수 있음
	ChangePhase(EBattlePhase::Ready);

	//카드 제시에서 Grid 범위 표시 비활성화 // TODO :: Delete This Line
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	if (!BattleMainScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("Widget_BattleMainScreen is null (BattleManager.cpp)"));
		return;
	}

	ReadyEnd();
}

void ABattleManager::ReadyEnd()
{
	//캐릭터 스폰
	CreateCharacter();

	BattleMainScreen->ReadyEnd();
	BattleStart();
}


//==========================================전투(Battle)================================================================
// ======== BattleStart() -> BattleMainScreen::BattleStart() -> ReadyEnd() -> BattleMainScreen:: ReadyEnd()================

void ABattleManager::BattleStart()
{
	if (GetCurrentPhase() == EBattlePhase::BattleStart)
	{
		return;
	}

	CurrentRound = 0;
	CurrentAttackActionIndex = INDEX_NONE;

	ChangePhase(EBattlePhase::BattleStart);
}

void ABattleManager::BattleEnd()
{
	ChangePhase(EBattlePhase::BattleEnd);
}

//===============================================국(Round)==============================================================
//국 시작	Round 시작
void ABattleManager::RoundStart()
{
	++CurrentRound;
	AttackActions.Empty();
	PlayerSelectAction.Empty();
	EnemySelectAction.Empty();

	ChangePhase(EBattlePhase::RoundStart);
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
}

//===============================================합(Exchange)===========================================================
void ABattleManager::ExchangeStart()
{
	CurrentExchange = 0;
	SetCurrentExchangeFlowState(EBattleExchangeFlowState::Idle);

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

	ChangePhase(EBattlePhase::ExchangeStart);
}

void ABattleManager::StartCurrentExchange()
{
	if (!BattleMainScreen || CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid current exchange: %d"), CurrentExchange);
		return;
	}

	bPlayerCardSelectionFinished = false;
	bEnemyCardSelectionFinished = false;
	SetCurrentExchangeFlowState(EBattleExchangeFlowState::CardSelecting);

	BattleMainScreen->StartExchange(CurrentExchange);
}

void ABattleManager::NotifyPlayerCardSelectionFinished()
{
	bPlayerCardSelectionFinished = true;
	TryBeginCurrentExchangeCardReveal();
}

void ABattleManager::NotifyEnemyCardSelectionFinished()
{
	bEnemyCardSelectionFinished = true;
	TryBeginCurrentExchangeCardReveal();
}

void ABattleManager::TryBeginCurrentExchangeCardReveal()
{
	if (!bPlayerCardSelectionFinished || !bEnemyCardSelectionFinished)
	{
		return;
	}

	if (CurrentExchangeFlowState != EBattleExchangeFlowState::Targeting)
	{
		return;
	}

	if (!PrepareCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to prepare exchange simulation: %d"), CurrentExchange);
		return;
	}

	SetCurrentExchangeFlowState(EBattleExchangeFlowState::CardRevealing);

	if (!BattleMainScreen->RevealEnemySelectedCard(CurrentExchange))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Failed to reveal enemy card. Exchange: %d"), CurrentExchange);
		NotifyEnemyCardRevealFinished(CurrentExchange);
	}
}

void ABattleManager::AdvanceExchange()
{
	if (CurrentExchange + 1 >= MaxExchangeCount)
	{
		ExchangeEnd();
		return;
	}

	++CurrentExchange;
	StartCurrentExchange();
}

void ABattleManager::ExchangeEnd()
{
	SetCurrentExchangeFlowState(EBattleExchangeFlowState::Idle);
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	ChangePhase(EBattlePhase::ExchangeEnd);
}

bool ABattleManager::PrepareCurrentExchangeSimulation()
{
	if (!BattleSimulationManager)
	{
		return false;
	}

	SetPlayerBattleAction();

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

bool ABattleManager::StartCurrentExchangeSimulation()
{
	if (!BattleSimulationManager)
	{
		return false;
	}

	if (CurrentExchangeFlowState != EBattleExchangeFlowState::CardRevealing)
	{
		return false;
	}

	SetCurrentExchangeFlowState(EBattleExchangeFlowState::Simulating);

	if (!BattleSimulationManager->ExecuteCurrentExchange())
	{
		SetCurrentExchangeFlowState(EBattleExchangeFlowState::CardRevealing);
		return false;
	}

	return true;
}

void ABattleManager::NotifyEnemyCardRevealFinished(int32 ExchangeIndex)
{
	if (ExchangeIndex != CurrentExchange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Reveal exchange mismatch. Current: %d, Finished: %d"), CurrentExchange, ExchangeIndex);
		return;
	}

	if (!StartCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to execute exchange simulation: %d"), CurrentExchange);
	}
}

void ABattleManager::HandleSimulationExchangeFinished(int32 FinishedExchangeIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[BattleManager] Simulation exchange finished: %d"), FinishedExchangeIndex);

	FinishCurrentExchangePresentation(FinishedExchangeIndex);
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

void ABattleManager::FinishCurrentExchangePresentation(int32 FinishedExchangeIndex)
{
	if (!BattleMainScreen)
	{
		return;
	}

	if (CurrentExchangeFlowState != EBattleExchangeFlowState::Simulating || FinishedExchangeIndex != CurrentExchange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Finished exchange mismatch. Current: %d, Finished: %d"), CurrentExchange, FinishedExchangeIndex);
		return;
	}

	SetCurrentExchangeFlowState(EBattleExchangeFlowState::Idle);
	BattleMainScreen->FinishExchange(FinishedExchangeIndex);
}

void ABattleManager::ExchangeCardTargeting(UMuksiBattleCardDataAsset* ExchangeCard)
{
	if (StartCurrentCardTargeting(ExchangeCard))
	{
		SetCurrentExchangeFlowState(EBattleExchangeFlowState::Targeting);
	}
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

bool ABattleManager::SetEnemyBattleAction()
{
	if (!EnemyBattleCharacter)
	{
		return false;
	}

	FBattleAction BattleAction;

	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = EnemyBattleCharacter->GetSelectEnemyCardDataAsset(BattleGridManager, this);
	BattleAction.Attacker = EnemyBattleCharacter;
	BattleAction.bPlayerAction = false;

	if (BattleAction.Card == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]EnemyBattleCharacter->GetSelectEnemyCardDataAsset is null!"));
		return false;
	}
	//좌표 구하는거
	BattleAction.Speed = EnemyBattleCharacter->GetCharacterSpeed() + BattleAction.Card->CardSpeed;
	BattleAction.TargetingResult.AffectedCoords = EnemyBattleCharacter->GetSelectEnemyCardCoord();

	EnemySelectAction.SetNum(CurrentExchange + 1);
	EnemySelectAction[CurrentExchange] = BattleAction;

	return true;
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
	if (AttackActions.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackStart: 실행할 공격 행동이 없습니다."));
		AttackEnd();
		return;
	}

	SortAttackActions();
	CurrentAttackActionIndex = 0;

	UE_LOG(LogTemp, Log, TEXT("AttackStart: 총 행동 개수 %d"), AttackActions.Num());
	ChangePhase(EBattlePhase::AttackStart);
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
	if (!IsValid(BattleSequenceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSequenceManager is null"));
		NotifyAttackActionFinished();
		return;
	}

	BattleSequenceManager->OnSequenceFinished.RemoveAll(this);
	BattleSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleManager::NotifyAttackActionFinished);

	if (!BattleSequenceManager->StartSequence(Action))
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
	UE_LOG(LogTemp, Log, TEXT("AttackEnd: All attack actions finished"));

	CurrentAttackActionIndex = INDEX_NONE;
	AttackActions.Empty();

	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
	}

	ChangePhase(EBattlePhase::AttackEnd);
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

UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Player(int32 ExchangeCount)
{
	if (!PlayerSelectAction.IsValidIndex(ExchangeCount))
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
	if (!EnemySelectAction.IsValidIndex(ExchangeCount))
	{
		UE_LOG(LogTemp, Error, TEXT("EnemySelectAction size : %d  Exchange Count %d"), EnemySelectAction.Num(), ExchangeCount);
		return nullptr;
	}
	UMuksiBattleCardDataAsset* Card = EnemySelectAction[ExchangeCount].Card;
	if (!Card)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]GetBattleCardDataAssetToExchange_Enemy is Null!!!"));
		return nullptr;
	}
	return Card;
}