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
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
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
	//TODO:: RefactoringTargetingManager : UObject -> Actor , DeleteDeveloperSettings
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

FHexOffsetCoord ABattleManager::GetPlayerCoord() const
{
	return PlayerBattleCharacter->GetCharacterCoord();
}

FHexOffsetCoord ABattleManager::GetEnemyCoord() const
{
	return EnemyBattleCharacter->GetCharacterCoord();
}

void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	if (CurrentPhase == NewPhase)
	{
		UE_LOG(LogTemp,Error,TEXT("[BattleManager]: You Try to Change Same Battle Phase!"))
		return;
	}

	const EBattlePhase OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	OnBattlePhaseChanged.Broadcast(OldPhase, CurrentPhase);
}

void ABattleManager::ChangeExchangePhase(EBattleExchangePhase NewState)
{
	if (CurrentExchangePhase == NewState)
	{
		return;
	}

	CurrentExchangePhase = NewState;
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

	BattleGridManager->PlaceCharacter(PlayerBattleCharacter, StartPlayerCoord);
	BattleGridManager->PlaceCharacter(EnemyBattleCharacter, StartEnemyCoord);
}

//===========================================준비(Ready)================================================================
// ======== ReadyStart() -> BattleMainScreen::ReadyStart() -> ReadyEnd() -> BattleMainScreen:: ReadyEnd()->BattleStart()================
//게임 실행 첫 프레임 이내로 끝남
void ABattleManager::ReadyStart()
{
	if (!BattleMainScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]Widget_BattleMainScreen is null."));
		return;
	}
	if (!BattleTargetingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]BattleTargetingManager is null."));
		return;
	}

	ChangePhase(EBattlePhase::Ready);

	BattleMainScreen->ReadyStart();

	ReadyEnd();
}

void ABattleManager::ReadyEnd()
{
	//캐릭터 스폰
	CreateCharacter();

	ChangePhase(EBattlePhase::ReadyEnd);

	BattleMainScreen->ReadyEnd();

	BattleStart();
}


//==========================================전투(Battle)================================================================
// ======== BattleStart() -> BattleMainScreen::BattleStart() -> BattleEnd() -> BattleMainScreen:: BattleEnd() -> RoundStart()================

void ABattleManager::BattleStart()
{
	CurrentRound = 0;
	CurrentAttackActionIndex = INDEX_NONE;

	ChangePhase(EBattlePhase::BattleStart);

	BattleMainScreen->BattleStart();

	BattleEnd();
}

void ABattleManager::BattleEnd()
{
	ChangePhase(EBattlePhase::BattleEnd);

	BattleMainScreen->BattleEnd();

	RoundStart();
}

//===============================================국(Round)==============================================================
// ======== RoundStart() -> BattleMainScreen::RoundStart() -> DisplayRoundStartUIFinish()->ExchangeStart()
 
void ABattleManager::RoundStart()
{
	++CurrentRound;
	AttackActions.Empty();
	PlayerSelectAction.Empty();
	EnemySelectAction.Empty();

	ChangePhase(EBattlePhase::RoundStart);

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
}

//===============================================합(Exchange)===========================================================
// ExchangeStart() -> BattleMainScreen::ExchangeStart()-> BattleMainScreen::DisplayExchangeStartUIFinish()->StartExchangeSelectCard()

void ABattleManager::ExchangeStart()
{
	CurrentExchange = 0;

	if (!BattleSimulationManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSimulationManager is null"));
		return;
	}

	TArray<ABattleCharacterBase*> SourceCharacters;
	SourceCharacters.Add(PlayerBattleCharacter);
	SourceCharacters.Add(EnemyBattleCharacter);

	BattleSimulationManager->OnSimulationExchangeFinished.RemoveAll(this);
	BattleSimulationManager->OnSimulationExchangeFinished.AddUObject(this, &ABattleManager::HandleExchangeSimulationFinished);
	BattleSimulationManager->StartSimulation(BattleGridManager, SourceCharacters);

	ChangePhase(EBattlePhase::ExchangeStart);
	ChangeExchangePhase(EBattleExchangePhase::Idle);

	BattleMainScreen->ExchangeStart();
}
void ABattleManager::AdvanceExchange()
{
	if (CurrentExchange + 1 >= MaxExchangeCount)
	{
		ExchangeEnd();
		return;
	}

	++CurrentExchange;
	StartExchangeSelectCard();
}
void ABattleManager::ExchangeEnd()
{
	ChangeExchangePhase(EBattleExchangePhase::Idle);
	ChangePhase(EBattlePhase::ExchangeEnd);
}

//=========================================카드 선택(Exchange_SelectCard)=======================================
// PlayerSelect_Prep : StartExchangeSelectCard->BattleMainScreen::StartExchangeSelectCard->BattleMainScreen::HandleExchangeSlot()->HandWidget::EnableExchangeSlot()
// Player_Select : BattleCardBase::StopDragging() -> CardEquipSlot::EquipCard()
// Player_Select_Cancel : CardEquipSlot::NativeOnMouseButtonDown() -> CardEquipSlot:: UnequipCard()
// Player_Select_Confirm : HandWidget::HandleEndTurnButtonClicked()-> UWidget_BattleMainScreen::HandleCardSelect -> UWidget_BattleMainScreen:: SelectCardDataSend()-> UWidget_BattleMainScreen::StartTargeting();
// Player_Select_Confirm_Cancel : None?
//EnemySelect: StartExchangeSelectCard->BattleMainScreen::StartExchangeSelectCard->BattleMainScreen::EnemySelectCardEvent()-> BattleMainScreen::EnemyPlaceCard()->NotifyEnemyCardSelectionFinished();

void ABattleManager::StartExchangeSelectCard()
{	
	if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid current exchange: %d"), CurrentExchange);
		return;
	}

	bPlayerCardSelectionFinished = false;
	bEnemyCardSelectionFinished = false;

	ChangeExchangePhase(EBattleExchangePhase::CardSelecting);

	BattleMainScreen->StartExchangeSelectCard(CurrentExchange);
}

//=========================================선택한 카드 타겟팅(Exchange_Targeting)=======================================
//PlayerTargeting : PlayerMode_Battle::HandleLeftClick()->ConfirmCurrentCardTargeting()-> NotifyPlayerCardSelectionFinished()
//EnemyTargeting : BattleMainScreen::EnemyPlaceCard()->NotifyEnemyCardSelectionFinished()

bool ABattleManager::StartTargeting(UMuksiBattleCardDataAsset* CardData)
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

	ChangeExchangePhase(EBattleExchangePhase::Targeting);

	CurrentCardDataAsset = CardData;

	BattleTargetingManager->StartTargeting(TargetingSourceCharacter, BattleGridManager, CardData->TargetingData);

	return BattleTargetingManager->IsTargeting();
}

//PlayerMode_Battle::TickPlayerMode
bool ABattleManager::UpdateCurrentCardTargeting(const FTargetingInputContext& InputContext)
{
	return BattleTargetingManager->UpdateTargeting(InputContext);
}
//PlayerMode_Battle::HandleLeftClick
bool ABattleManager::ConfirmCurrentCardTargeting()
{
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
//PlayerMode_Battle::HandleRightClick
void ABattleManager::CancelCurrentCardTargeting()
{
	if (BattleTargetingManager)
	{
		BattleTargetingManager->CancelTargeting();
	}

	if (CurrentPhase == EBattlePhase::ExchangeStart && CurrentExchangePhase == EBattleExchangePhase::Targeting && !bPlayerCardSelectionFinished)
	{
		ChangeExchangePhase(EBattleExchangePhase::CardSelecting);
	}
}

bool ABattleManager::IsCardTargeting() const
{
	return BattleTargetingManager && BattleTargetingManager->IsTargeting();
}

//=========================================카드 공개(Exchange_RevealCard)=======================================
//
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

	if (CurrentExchangePhase != EBattleExchangePhase::Targeting)
	{
		return;
	}

	if (!PrepareCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to prepare exchange simulation: %d"), CurrentExchange);
		return;
	}

	ChangeExchangePhase(EBattleExchangePhase::CardRevealing);

	if (!BattleMainScreen->RevealEnemySelectedCard(CurrentExchange))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Failed to reveal enemy card. Exchange: %d"), CurrentExchange);
		NotifyEnemyCardRevealFinished(CurrentExchange);
	}
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


//=========================================시뮬레이션(Exchange_Simulation)=======================================

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
	if (CurrentExchangePhase != EBattleExchangePhase::CardRevealing)
	{
		return false;
	}

	ChangeExchangePhase(EBattleExchangePhase::Simulating);

	if (!BattleSimulationManager->ExecuteCurrentExchange())
	{
		ChangeExchangePhase(EBattleExchangePhase::CardRevealing);
		return false;
	}

	return true;
}


void ABattleManager::HandleExchangeSimulationFinished(int32 FinishedExchangeIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[BattleManager] Simulation exchange finished: %d"), FinishedExchangeIndex);

	if (!BattleMainScreen)
	{
		return;
	}

	if (CurrentExchangePhase != EBattleExchangePhase::Simulating || FinishedExchangeIndex != CurrentExchange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Finished exchange mismatch. Current: %d, Finished: %d"), CurrentExchange, FinishedExchangeIndex);
		return;
	}

	ChangeExchangePhase(EBattleExchangePhase::Idle);
	BattleMainScreen->FinishExchange(FinishedExchangeIndex);
}

//==========================================================

void ABattleManager::SetPlayerBattleAction()
{
	if (!BattleTargetingManager || !PlayerBattleCharacter || !CurrentCardDataAsset)
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
	BattleAction.Card = CurrentCardDataAsset;
	BattleAction.Speed = PlayerBattleCharacter->GetCharacterSpeed() + CurrentCardDataAsset->CardSpeed;
	BattleAction.Attacker = PlayerBattleCharacter;
	BattleAction.bPlayerAction = true;
	BattleAction.TargetingResult = TargetingResult;

	ABattleCharacterBase* PlayerTargetingSource = GetCurrentTargetingSourceCharacter();

	if (!PlayerTargetingSource)
	{
		return;
	}

	BattleAction.TargetingIntent = FBattleTargetResolver::CaptureIntent(
		BattleAction.TargetingResult,
		PlayerTargetingSource->GetCharacterCoord());

	PlayerSelectAction.SetNum(CurrentExchange + 1);
	PlayerSelectAction[CurrentExchange] = BattleAction;

	UE_LOG(LogTemp, Log, TEXT("SetPlayerBattleAction exchange: %d"), CurrentExchange);
}

bool ABattleManager::SetEnemyBattleAction()
{
	if (!EnemyBattleCharacter || !BattleGridManager)
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

	for (const FHexOffsetCoord& TargetCoord : BattleAction.TargetingResult.AffectedCoords)
	{
		const FBattleGridCell* TargetCell = BattleGridManager->GetCellByCoord(TargetCoord);

		if (!TargetCell)
		{
			continue;
		}

		if (ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(TargetCell->OccupyingActor.Get()))
		{
			BattleAction.TargetingResult.TargetCharacters.AddUnique(TargetCharacter);
		}
	}

	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		BattleSimulationManager->ConvertToSourceTargetingResult(BattleAction.TargetingResult);
	}

	ABattleCharacterBase* EnemyTargetingSource = ResolveCurrentTargetingCharacter(EnemyBattleCharacter);

	if (!EnemyTargetingSource)
	{
		return false;
	}

	BattleAction.TargetingIntent = FBattleTargetResolver::CaptureIntent(
		BattleAction.TargetingResult,
		EnemyTargetingSource->GetCharacterCoord()
	);

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
