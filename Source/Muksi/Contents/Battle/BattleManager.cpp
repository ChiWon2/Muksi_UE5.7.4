#include "Muksi/Contents/Battle/BattleManager.h"

#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "Character/BattleCharacter_Enemy.h"
#include "Character/BattleStatComponent.h"
#include "Engine/TargetPoint.h"
#include "Grid/BattleGridManager.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"

// ============================================================================
// 생명주기 및 전투 진입
// 호출 흐름: BeginPlay -> ReadyStart -> UI ReadyEnd -> BattleStart
// ============================================================================


ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	TargetingPresentationController = NewObject<UTargetingPresentationController>(this);
	if (TargetingPresentationController)
	{
		TargetingPresentationController->Initialize(BattleGridManager);
	}

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleManager>(this);
	}
}


void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndTargetingSessions();

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

void ABattleManager::ReadyStart()
{
	if (!BattleMainScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]Widget_BattleMainScreen is null."));
		return;
	}
	ChangePhase(EBattlePhase::Ready);

	//적 캐릭터 정보 얻어오기
	GetEnemyData();

	BattleMainScreen->ReadyStart();

	ReadyEnd();
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

	checkf(IsValid(PlayerBattleCharacter), TEXT("PlayerBattleCharacter Spawn Error BattleManager"));
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
	EnemyBattleCharacter->SetCharacterData(TestEnemyCharacterDataAsset, this, BattleMainScreen);

	BattleGridManager->PlaceCharacter(PlayerBattleCharacter, StartPlayerCoord);
	BattleGridManager->PlaceCharacter(EnemyBattleCharacter, StartEnemyCoord);
}


bool ABattleManager::StartCurrentCardTargeting(UMuksiBattleCardDataAsset* CardData)
{
	CurrentRound = 0;
	GetWorldTimerManager().ClearTimer(NextAttackActionTimerHandle);
	GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
	bAttackActionCompletionPending = false;
	CurrentAttackActionIndex = INDEX_NONE;

	/*ChangePhase(EBattlePhase::BattleStart);

	BattleMainScreen->BattleStart();

	BattleEnd();*/
	
	return true;
}

// ============================================================================
// Round 파이프라인
// 호출 흐름: BattleStart -> RoundStart -> ExchangeStart / RoundEnd -> 다음 Round 또는 BattleEnd
// ============================================================================

void ABattleManager::GetEnemyData()
{
	UBattleEncounterSubsystem* BattleEncounterSubsystem =UBattleEncounterSubsystem::Get(this);

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
		TestEnemyCharacterDataAsset = EnemyCharacterData;
		return;
	}
}


void ABattleManager::ReadyEnd()
{
	//캐릭터 스폰
	CreateCharacter();
	BindingBattleEndEvent();
	ChangePhase(EBattlePhase::ReadyEnd);

	BattleMainScreen->ReadyEnd();

	BattleStart();
}

void ABattleManager::BindingBattleEndEvent()
{
	PlayerBattleCharacter->GetBattleStatComponent()->OnDead.AddUniqueDynamic(this, &ABattleManager::CharacterDeadPoint);
	EnemyBattleCharacter->GetBattleStatComponent()->OnDead.AddUniqueDynamic(this, &ABattleManager::CharacterDeadPoint);
}

void ABattleManager::BattleStart()
{
	CurrentRound = 0;
	GetWorldTimerManager().ClearTimer(NextAttackActionTimerHandle);
	GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
	bAttackActionCompletionPending = false;
	CurrentAttackActionIndex = INDEX_NONE;

	ChangePhase(EBattlePhase::BattleStart);

	BattleMainScreen->BattleStart();
	
}


void ABattleManager::BattleEnd()
{
	ChangePhase(EBattlePhase::BattleEnd);
	UnbindingHandler();
	
	
	BattleMainScreen->BattleEnd();
	//RoundStart();
}

void ABattleManager::EndBattleLevel()
{
	UBattleEncounterSubsystem* EncounterSubsystem = UBattleEncounterSubsystem::Get(this);
	if (!IsValid(EncounterSubsystem))
	{
		return;
	}

	FBattleResult BattleResult;
	//TODO 전투 종료 후 체력, 경험치(?)등등 영수증 작성하기
	EncounterSubsystem->FinishBattleEncounter(
		BattleResult
	);
}
void ABattleManager::UnbindingHandler()
{
	GetWorld()->GetTimerManager().ClearTimer(NextAttackActionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
	
}

// ============================================================================
// Round 파이프라인
// 호출 흐름: BattleStart -> RoundStart -> ExchangeStart / RoundEnd -> 다음 Round
// ============================================================================




void ABattleManager::RoundStart()
{
	++CurrentRound;
	EndTargetingSessions();
	AttackActionQueue.Empty();
	PlayerExchangeActions.Empty();
	EnemyExchangeActions.Empty();

	ChangePhase(EBattlePhase::RoundStart);

	BattleMainScreen->RoundStart();
}

void ABattleManager::CharacterDeadPoint(ABattleCharacterBase* Character)
{
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst1"));
	//각 캐릭터별 전용 엔딩이 있으면 그걸로
	if (bIsCharacterDead) { return; }
	bIsCharacterDead = true;
	UE_LOG(LogTemp, Error, TEXT("EndBattleLevel TEst2"));
	//일단은 그냥 넘겨
	BattleEnd();
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

	// RoundEnd UI owns card-slot cleanup, hand reset, and the transition to the next round.
	if (BattleMainScreen)
	{
		BattleMainScreen->RoundEnd();
	}
}


bool ABattleManager::ShouldEndBattle() const
{
	// 테스트용: 1국만 진행하고 전투 종료
	return CurrentRound >= 1;

	// 나중에는 이런 식으로 교체 가능:
	// return !PlayerCharacterData || !EnemyCharacterData || PlayerCharacterData->IsDead() || EnemyCharacterData->IsDead();
}

// ============================================================================
// Exchange 시작 및 카드 선택
// Player: BattleMainScreen 카드 선택 -> StartPlayerCardTargeting -> Confirm... -> NotifyPlayerCardSelectionFinished
// Enemy: BattleMainScreen::EnemyPlaceCard -> NotifyEnemyCardSelectionFinished -> TryBeginCurrentExchangeCardReveal
// ============================================================================


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
	BattleSimulationManager->OnBattleSimulationFinished.RemoveAll(this);
	BattleSimulationManager->OnBattleSimulationFinished.AddUObject(this, &ABattleManager::HandleBattleSimulationFinished);
	BattleSimulationManager->OnSimulationActionStarted.RemoveAll(this);
	BattleSimulationManager->OnSimulationActionStarted.AddUObject(this, &ABattleManager::HandleSimulationActionStarted);
	BattleSimulationManager->OnSimulationExecutionStarted.RemoveAll(this);
	BattleSimulationManager->OnSimulationExecutionStarted.AddUObject(this, &ABattleManager::HandleSimulationExecutionStarted);
	BattleSimulationManager->OnSimulationActionFinished.RemoveAll(this);
	BattleSimulationManager->OnSimulationActionFinished.AddUObject(this, &ABattleManager::HandleSimulationActionFinished);

	if (!BattleSimulationManager->StartSimulation(BattleGridManager, SourceCharacters))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to start battle simulation"));
		return;
	}

	ChangePhase(EBattlePhase::ExchangeStart);
	ChangeExchangePhase(EBattleExchangePhase::Idle);

	BattleMainScreen->ExchangeStart();
}


void ABattleManager::StartExchangeSelectCard()
{
	if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid current exchange: %d"), CurrentExchange);
		return;
	}

	bPlayerCardSelectionFinished = false;
	bEnemyCardSelectionFinished = false;
	EndTargetingSessions();
	PendingPlayerCard = nullptr;

	ChangeExchangePhase(EBattleExchangePhase::CardSelecting);

	BattleMainScreen->StartExchangeSelectCard(CurrentExchange);
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

	if (CurrentExchangePhase != EBattleExchangePhase::Targeting)
	{
		return;
	}

	if (!PrepareCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to prepare exchange simulation: %d"), CurrentExchange);
		bEnemyCardSelectionFinished = false;
		ChangeExchangePhase(EBattleExchangePhase::CardSelecting);
		return;
	}

	ChangeExchangePhase(EBattleExchangePhase::CardRevealing);

	if (!BattleMainScreen->RevealEnemySelectedCard(CurrentExchange))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Failed to reveal enemy card. Exchange: %d"), CurrentExchange);
		NotifyEnemyCardRevealFinished(CurrentExchange);
	}
}

// ============================================================================
// 선택한 카드 타겟팅 (Exchange_Targeting)
// 입력 흐름: PlayerMode_Battle::HandleLeftClick/RightClick -> BattleManager Confirm/Undo -> BattleTargetingSession
// ============================================================================


bool ABattleManager::StartPlayerCardTargeting(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData || !BattleGridManager)
	{
		return false;
	}

	// Runtime Simulation/Attack Sequence preview sessions must not overlap
	// with the interactive targeting preview. In particular, an old area
	// preview actor can hide or overwrite the newly-created cone preview.
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearExecutionPreview();
	}

	if (PlayerTargetingSession)
	{
		PlayerTargetingSession->EndSession();
	}

	ABattleCharacterBase* PlayerTargetingActor = GetPlayerTargetingActor();

	if (!PlayerTargetingActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to resolve player targeting actor"));
		return false;
	}

	ChangeExchangePhase(EBattleExchangePhase::Targeting);

	PendingPlayerCard = CardData;
	PlayerTargetingSession = NewObject<UBattleTargetingSession>(this);

	if (!PlayerTargetingSession || !PlayerTargetingSession->StartSession(PlayerTargetingActor, BattleGridManager, CardData->TargetingData, true))
	{
		PlayerTargetingSession = nullptr;
		PendingPlayerCard = nullptr;
		ChangeExchangePhase(EBattleExchangePhase::CardSelecting);
		return false;
	}

	if (PlayerTargetingSession->IsCompleted())
	{
		NotifyPlayerCardSelectionFinished();
	}

	return true;
}

bool ABattleManager::UpdatePlayerTargetingCandidate(const FHexOffsetCoord& CandidateCoord)
{
	return PlayerTargetingSession && PlayerTargetingSession->UpdateCandidateCoord(CandidateCoord);
}


void ABattleManager::UpdatePlayerTargetingAim(const FVector& AimWorldLocation, bool bHasAimLocation)
{
	if (PlayerTargetingSession)
	{
		PlayerTargetingSession->UpdateAimWorldLocation(AimWorldLocation, bHasAimLocation);
	}
}

ETargetingConfirmResult ABattleManager::ConfirmPlayerCardTargetingStep()
{
	if (!PlayerTargetingSession)
	{
		return ETargetingConfirmResult::Failed;
	}

	const ETargetingConfirmResult ConfirmResult = PlayerTargetingSession->ConfirmStep();

	if (ConfirmResult == ETargetingConfirmResult::Completed)
	{
		NotifyPlayerCardSelectionFinished();
	}

	return ConfirmResult;
}


bool ABattleManager::UndoPlayerCardTargetingStep()
{
	if (!PlayerTargetingSession
		|| CurrentPhase != EBattlePhase::ExchangeStart
		|| CurrentExchangePhase != EBattleExchangePhase::Targeting)
	{
		return false;
	}

	if (!PlayerTargetingSession->UndoStep())
	{
		return false;
	}

	// The player is editing targeting again. Any previous completion flag must
	// be revoked so card reveal cannot start from stale completion state.
	bPlayerCardSelectionFinished = false;

	UE_LOG(LogTemp, Log, TEXT("[Targeting] Right-click undo succeeded. RestoredStep=%d"),
		PlayerTargetingSession->GetCurrentStepIndex());
	return true;
}

void ABattleManager::CancelPlayerCardTargeting()
{
	if (PlayerTargetingSession)
	{
		PlayerTargetingSession->EndSession();
		PlayerTargetingSession = nullptr;
	}

	PendingPlayerCard = nullptr;

	if (CurrentPhase == EBattlePhase::ExchangeStart && CurrentExchangePhase == EBattleExchangePhase::Targeting && !bPlayerCardSelectionFinished)
	{
		ChangeExchangePhase(EBattleExchangePhase::CardSelecting);
	}
}


bool ABattleManager::IsPlayerCardTargeting() const
{
	return PlayerTargetingSession
		&& CurrentPhase == EBattlePhase::ExchangeStart
		&& CurrentExchangePhase == EBattleExchangePhase::Targeting
		&& (PlayerTargetingSession->IsSelecting() || PlayerTargetingSession->IsCompleted());
}


bool ABattleManager::HasActivePlayerTargetingSession() const
{
	return PlayerTargetingSession
		&& CurrentPhase == EBattlePhase::ExchangeStart
		&& CurrentExchangePhase == EBattleExchangePhase::Targeting;
}

// ============================================================================
// 카드 공개 및 Simulation 준비
// 호출 흐름: 양측 선택 완료 -> Action 생성 -> 공개 Preview -> NotifyEnemyCardRevealFinished -> Simulation 준비/실행
// ============================================================================


bool ABattleManager::BuildPlayerActionForCurrentExchange()
{
	if (!PlayerTargetingSession || !PlayerTargetingSession->IsCompleted() || !PlayerBattleCharacter || !PendingPlayerCard)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to create player battle action"));
		return false;
	}

	FBattleAction BattleAction;
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = PendingPlayerCard;
	BattleAction.Speed = PlayerBattleCharacter->GetCharacterSpeed() + PendingPlayerCard->CardSpeed;
	BattleAction.Attacker = PlayerBattleCharacter;
	BattleAction.bPlayerAction = true;
	BattleAction.TargetingIntent = PlayerTargetingSession->GetIntent();

	PlayerExchangeActions.SetNum(CurrentExchange + 1);
	PlayerExchangeActions[CurrentExchange] = BattleAction;

	UE_LOG(LogTemp, Log, TEXT("BuildPlayerActionForCurrentExchange exchange: %d"), CurrentExchange);
	return true;
}


bool ABattleManager::BuildEnemyActionForCurrentExchange()
{
	if (!EnemyBattleCharacter || !BattleGridManager)
	{
		return false;
	}

	UMuksiBattleCardDataAsset* SelectedCard = EnemyBattleCharacter->SelectCardForExchange(BattleGridManager, this);

	if (!SelectedCard)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Enemy card selection failed"));
		return false;
	}

	ABattleCharacterBase* EnemyTargetingActor = GetEnemyTargetingActor();
	ABattleCharacterBase* PlayerTargetingActor = GetPlayerTargetingActor();

	if (!EnemyTargetingActor || !PlayerTargetingActor)
	{
		return false;
	}

	// Keep hidden enemy targeting isolated from any runtime presentation
	// sessions left by the previous simulation/attack-sequence phase.
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearExecutionPreview();
	}

	if (EnemyTargetingSession)
	{
		EnemyTargetingSession->EndSession();
	}

	EnemyTargetingSession = NewObject<UBattleTargetingSession>(this);

	if (!EnemyTargetingSession || !EnemyTargetingSession->StartSession(EnemyTargetingActor, BattleGridManager, SelectedCard->TargetingData, false))
	{
		EnemyTargetingSession = nullptr;
		return false;
	}

	// Temporary deterministic enemy targeting: every targeting step aims at the player.
	// Preview is disabled while building the hidden intent; it is shown after card reveal via indicators.
	while (EnemyTargetingSession->IsSelecting())
	{
		const int32 StepIndex = EnemyTargetingSession->GetCurrentStepIndex();
		const FHexOffsetCoord PlayerCoord = PlayerTargetingActor->GetCharacterCoord();
		const FVector PlayerWorldLocation = BattleGridManager->GetWorldLocationByCoord(PlayerCoord);

		EnemyTargetingSession->UpdateAimWorldLocation(PlayerWorldLocation, true);

		if (!EnemyTargetingSession->UpdateCandidateCoord(PlayerCoord) ||
			EnemyTargetingSession->ConfirmStep() == ETargetingConfirmResult::Failed)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[BattleManager] Enemy card cannot target the player at step %d. Card: %s"),
				StepIndex,
				*GetNameSafe(SelectedCard));
			EnemyTargetingSession->EndSession();
			EnemyTargetingSession = nullptr;
			return false;
		}
	}

	if (!EnemyTargetingSession->IsCompleted())
	{
		return false;
	}

	FBattleAction BattleAction;
	BattleAction.ExchangeIndex = CurrentExchange;
	BattleAction.Card = SelectedCard;
	BattleAction.Attacker = EnemyBattleCharacter;
	BattleAction.Speed = EnemyBattleCharacter->GetCharacterSpeed() + SelectedCard->CardSpeed;
	BattleAction.bPlayerAction = false;
	BattleAction.TargetingIntent = EnemyTargetingSession->GetIntent();

	EnemyExchangeActions.SetNum(CurrentExchange + 1);
	EnemyExchangeActions[CurrentExchange] = BattleAction;

	return true;
}


void ABattleManager::RefreshExchangeTargetIndicators()
{
	if (!BattleGridManager || !PlayerExchangeActions.IsValidIndex(CurrentExchange) || !EnemyExchangeActions.IsValidIndex(CurrentExchange))
	{
		return;
	}

	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	const FBattleAction& PlayerBattleAction = PlayerExchangeActions[CurrentExchange];

	UE_LOG(LogTemp, Log, TEXT("Current Exchange num %d"), PlayerExchangeActions.Num());

	FResolvedTargeting PlayerResolvedTargeting;

	if (ResolveActionTargetingForCurrentGrid(PlayerBattleAction, PlayerResolvedTargeting))
	{
		TArray<FHexOffsetCoord> PlayerTargetCoords = PlayerResolvedTargeting.AffectedCoords;

		if (PlayerTargetCoords.IsEmpty() && PlayerResolvedTargeting.HasSelectedCoord())
		{
			PlayerTargetCoords.Add(PlayerResolvedTargeting.GetSelectedCoord());
		}

		BattleGridManager->SetExchangeIndicator(PlayerBattleAction.Card->AttackType.AttackType, PlayerTargetCoords, false);
	}

	const FBattleAction& EnemyBattleAction = EnemyExchangeActions[CurrentExchange];
	FResolvedTargeting EnemyResolvedTargeting;

	if (ResolveActionTargetingForCurrentGrid(EnemyBattleAction, EnemyResolvedTargeting))
	{
		TArray<FHexOffsetCoord> EnemyTargetCoords = EnemyResolvedTargeting.AffectedCoords;

		if (EnemyTargetCoords.IsEmpty() && EnemyResolvedTargeting.HasSelectedCoord())
		{
			EnemyTargetCoords.Add(EnemyResolvedTargeting.GetSelectedCoord());
		}

		BattleGridManager->SetExchangeIndicator(EnemyBattleAction.Card->AttackType.AttackType, EnemyTargetCoords, true);
	}
}


void ABattleManager::NotifyEnemyCardRevealFinished(int32 ExchangeIndex)
{
	if (ExchangeIndex != CurrentExchange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Reveal exchange mismatch. Current: %d, Finished: %d"), CurrentExchange, ExchangeIndex);
		return;
	}

	// The opponent target is intentionally revealed only after the card has flipped open.
	RefreshExchangeTargetIndicators();

	// Card Reveal도 세션의 오래된 단일-Step Preview를 재생하지 않고,
	// 정식 Resolver 결과와 Step별 AttackSequence Presentation 설정을 사용한다.
	if (TargetingPresentationController && EnemyExchangeActions.IsValidIndex(CurrentExchange))
	{
		TargetingPresentationController->ClearExecutionPreview();

		const FBattleAction& EnemyAction = EnemyExchangeActions[CurrentExchange];
		const int32 StepCount = IsValid(EnemyAction.Card) ? EnemyAction.Card->TargetingData.Steps.Num() : 0;
		for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
		{
			const FTargetingStepCardData* StepData = EnemyAction.Card->TargetingData.GetStep(StepIndex);
			if (!StepData)
			{
				continue;
			}

			const FTargetingPhasePresentationSettings& PresentationSettings =
				StepData->AdvancedSettings.Presentation.AttackSequencePhase;
			const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview
				|| PresentationSettings.bShowPathPreview
				|| PresentationSettings.bShowAreaPreview;
			if (!bShowAnyPreview)
			{
				continue;
			}

			FResolvedTargeting StepResolvedTargeting;
			if (ResolveActionTargetingThroughStepForCurrentGrid(EnemyAction, StepIndex, StepResolvedTargeting))
			{
				TargetingPresentationController->AddResolvedStepPreview(
					EnemyAction.Attacker,
					EnemyAction.Card->TargetingData,
					StepResolvedTargeting,
					StepIndex,
					PresentationSettings,
					true);
			}
		}
	}

	// Keep the resolved enemy Step previews on screen before simulation begins.
	GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
	GetWorldTimerManager().SetTimer(
		EnemyPreviewHideTimerHandle,
		this,
		&ABattleManager::HideEnemyTargetingPreviewAndStartSimulation,
		FMath::Max(EnemyPreviewDuration, 0.05f),
		false);
}


void ABattleManager::HideEnemyTargetingPreview()
{
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearExecutionPreview();
	}

	if (EnemyTargetingSession)
	{
		EnemyTargetingSession->HidePreview();
	}
}


void ABattleManager::HideEnemyTargetingPreviewAndStartSimulation()
{
	HideEnemyTargetingPreview();

	// Card-reveal indicators describe the hidden intent only. Clear them before
	// simulation, where targets are resolved again against the mutable simulation grid.
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}

	if (!StartCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to execute exchange simulation after enemy preview: %d"), CurrentExchange);
	}
}


bool ABattleManager::PrepareCurrentExchangeSimulation()
{
	if (!BattleSimulationManager)
	{
		return false;
	}

	if (!BuildPlayerActionForCurrentExchange() || !PlayerExchangeActions.IsValidIndex(CurrentExchange))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Player action is invalid. Exchange: %d"), CurrentExchange);
		return false;
	}

	if (!EnemyExchangeActions.IsValidIndex(CurrentExchange))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Enemy action is invalid. Exchange: %d"), CurrentExchange);
		return false;
	}

	// Keep the completed targeting sessions alive until the enemy card has been revealed.
	// The hidden intent remains stored in the action; reveal previews are rebuilt by the resolver.
	if (!BattleSimulationManager->SetEnemyAction(EnemyExchangeActions[CurrentExchange]))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to set enemy simulation action. Exchange: %d"), CurrentExchange);
		return false;
	}

	if (!BattleSimulationManager->SetPlayerAction(PlayerExchangeActions[CurrentExchange]))
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

	// 선택/카드 공개 단계에서 생성된 Preview는 Simulation 좌표와 무관하므로
	// 첫 Runtime Execution Preview를 만들기 전에 반드시 제거한다.
	ClearSelectionAndRevealPreviews();

	ChangeExchangePhase(EBattleExchangePhase::Simulating);

	if (!BattleSimulationManager->ExecuteCurrentExchange())
	{
		ChangeExchangePhase(EBattleExchangePhase::CardRevealing);
		return false;
	}

	return true;
}

// ============================================================================
// Simulation 콜백 및 Exchange 종료
// 콜백 흐름: BattleSimulationManager Action/Execution 이벤트 -> Preview 갱신 -> Exchange 완료 -> AdvanceExchange
// ============================================================================




void ABattleManager::HandleSimulationActionStarted(const FBattleAction& Action)
{
	(void)Action;
	// Selection/reveal visuals are stale once the simulation action begins.
	ClearRuntimeSimulationPreview();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
}


void ABattleManager::HandleSimulationExecutionStarted(
	const FBattleAction& Action,
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	const FResolvedTargeting& ResolvedTargeting)
{
	(void)Entry;
	(void)EntryIndex;
	RefreshRuntimeTargetingPresentation(Action, false, ResolvedTargeting);
}


void ABattleManager::HandleAttackSequenceExecutionStarted(
	const FBattleAction& Action,
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	const FResolvedTargeting& ResolvedTargeting)
{
	(void)Entry;
	(void)EntryIndex;
	RefreshRuntimeTargetingPresentation(Action, true, ResolvedTargeting);
}


void ABattleManager::RefreshRuntimeTargetingPresentation(
	const FBattleAction& Action,
	bool bAttackSequencePhase,
	const FResolvedTargeting& ExecutionResolvedTargeting)
{
	ClearRuntimeSimulationPreview();

	if (!BattleGridManager || !IsValid(Action.Card))
	{
		return;
	}

	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	ABattleCharacterBase* RuntimeAttacker = Action.Attacker;
	if (!bAttackSequencePhase && BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		if (ABattleSimulationCharacter* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(Action.Attacker))
		{
			RuntimeAttacker = SimulationCharacter;
		}
	}

	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData)
		{
			continue;
		}

		FResolvedTargeting StepResolvedTargeting;
		if (StepIndex == StepCount - 1)
		{
			StepResolvedTargeting = ExecutionResolvedTargeting;
		}
		else if (!ResolveActionTargetingThroughStepForCurrentGrid(Action, StepIndex, StepResolvedTargeting))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RuntimeTargetingPresentation] Resolve failed. Phase=%s Step=%d"),
				bAttackSequencePhase ? TEXT("AttackSequence") : TEXT("Simulation"), StepIndex);
			continue;
		}

		const FTargetingPhasePresentationSettings& PresentationSettings = bAttackSequencePhase
			? StepData->AdvancedSettings.Presentation.AttackSequencePhase
			: StepData->AdvancedSettings.Presentation.SimulationPhase;

		if (PresentationSettings.bShowIndicator)
		{
			TArray<FHexOffsetCoord> StepIndicatorCoords = StepResolvedTargeting.AffectedCoords;
			if (StepIndicatorCoords.IsEmpty())
			{
				if (const FTargetingStepResult* StepResult = StepResolvedTargeting.GetStep(StepIndex))
				{
					if (StepResult->HasSelectedCoord())
					{
						StepIndicatorCoords.Add(StepResult->SelectedCoord);
					}
				}
			}

			for (const FHexOffsetCoord& Coord : StepIndicatorCoords)
			{
				IndicatorCoords.AddUnique(Coord);
			}
		}

		const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview
			|| PresentationSettings.bShowPathPreview
			|| PresentationSettings.bShowAreaPreview;
		if (bShowAnyPreview && TargetingPresentationController)
		{
			TargetingPresentationController->AddResolvedStepPreview(
				RuntimeAttacker,
				Action.Card->TargetingData,
				StepResolvedTargeting,
				StepIndex,
				PresentationSettings,
				!Action.bPlayerAction);
		}
	}

	if (!IndicatorCoords.IsEmpty())
	{
		BattleGridManager->SetExchangeIndicator(
			Action.Card->AttackType.AttackType,
			IndicatorCoords,
			!Action.bPlayerAction);
	}
}


void ABattleManager::HandleSimulationActionFinished()
{
	ClearRuntimeSimulationPreview();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
}


void ABattleManager::ClearRuntimeSimulationPreview()
{
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearExecutionPreview();
	}
}


void ABattleManager::ClearSelectionAndRevealPreviews()
{
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearAll(PlayerTargetingSession, EnemyTargetingSession);
		return;
	}

	EndTargetingSessions();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
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

	// 한 Simulation 페이즈가 끝나면 해당 페이즈의 선택/교환 표시를 즉시 정리한다.
	EndTargetingSessions();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}

	ChangeExchangePhase(EBattleExchangePhase::Idle);
	BattleMainScreen->FinishExchange(FinishedExchangeIndex);
}


void ABattleManager::HandleBattleSimulationFinished()
{
	if (!BattleSimulationManager)
	{
		return;
	}

	AttackActionQueue = BattleSimulationManager->GetSequenceActionQueue();

	if (AttackActionQueue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Simulation completed without sequence actions."));
		AttackEnd();
		return;
	}

	// Simulation is complete. Continue into the real attack sequence immediately.
	AttackStart();
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

	if (BattleMainScreen)
	{
		BattleMainScreen->ExchangeEnd();
	}
}

// ============================================================================
// 실제 공격 재생 파이프라인
// 호출 흐름: AttackStart -> StartCurrentAttackAction -> BattleSequenceManager -> NotifyAttackActionFinished -> 다음 Action/AttackEnd
// ============================================================================

void ABattleManager::SortAttackActionQueue()
{
	AttackActionQueue.Sort([](const FBattleAction& A, const FBattleAction& B)
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
	if (AttackActionQueue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackStart: 실행할 공격 행동이 없습니다."));
		AttackEnd();
		return;
	}

	SortAttackActionQueue();
	CurrentAttackActionIndex = 0;
	bAttackActionCompletionPending = false;

	UE_LOG(LogTemp, Log, TEXT("AttackStart: 총 행동 개수 %d"), AttackActionQueue.Num());
	ChangePhase(EBattlePhase::AttackStart);

	if (BattleMainScreen)
	{
		BattleMainScreen->AttackStart();
	}
	else
	{
		StartCurrentAttackAction();
	}
}

void ABattleManager::StartCurrentAttackAction()
{
	if (!AttackActionQueue.IsValidIndex(CurrentAttackActionIndex))
	{
		AttackEnd();
		return;
	}

	const FBattleAction& CurrentAction = AttackActionQueue[CurrentAttackActionIndex];
	if (!IsValid(CurrentAction.Attacker) || !IsValid(CurrentAction.Card))
	{
		FinishCurrentAttackAction();
		return;
	}

	//공격카드 사용 시작 이벤트 
	OnAttackActionStarted.Broadcast(CurrentAction);
	
	if (!IsValid(BattleSequenceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] BattleSequenceManager is null"));
		NotifyAttackActionFinished();
		return;
	}

	BattleSequenceManager->OnSequenceFinished.RemoveAll(this);
	BattleSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleManager::NotifyAttackActionFinished);
	BattleSequenceManager->OnExecutionEntryStarted.RemoveAll(this);
	BattleSequenceManager->OnExecutionEntryStarted.AddUObject(this, &ABattleManager::HandleAttackSequenceExecutionStarted);

	if (!BattleSequenceManager->StartSequence(CurrentAction))
	{
		BattleSequenceManager->OnSequenceFinished.RemoveAll(this);
		NotifyAttackActionFinished();
	}
}


void ABattleManager::NotifyAttackActionFinished()
{
	if (BattleSequenceManager)
	{
		BattleSequenceManager->OnExecutionEntryStarted.RemoveAll(this);
	}
	ClearRuntimeSimulationPreview();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}

	if (bAttackActionCompletionPending)
	{
		return;
	}

	bAttackActionCompletionPending = true;

	if (!AttackActionQueue.IsValidIndex(CurrentAttackActionIndex))
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

	if (!AttackActionQueue.IsValidIndex(CurrentAttackActionIndex))
	{
		bAttackActionCompletionPending = false;
		AttackEnd();
		return;
	}

	GetWorldTimerManager().ClearTimer(NextAttackActionTimerHandle);
	NextAttackActionTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ABattleManager::StartNextAttackActionDeferred));
}


void ABattleManager::StartNextAttackActionDeferred()
{
	bAttackActionCompletionPending = false;
	StartCurrentAttackAction();
}


void ABattleManager::AttackEnd()
{
	UE_LOG(LogTemp, Log, TEXT("AttackEnd: All attack actions finished"));

	GetWorldTimerManager().ClearTimer(NextAttackActionTimerHandle);
	GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
	bAttackActionCompletionPending = false;
	CurrentAttackActionIndex = INDEX_NONE;
	AttackActionQueue.Empty();

	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
	}

	ChangePhase(EBattlePhase::AttackEnd);

	if (BattleMainScreen)
	{
		BattleMainScreen->AttackEnd();
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

// ============================================================================
// 타겟팅 해석 및 공통 지원
// 공통 계산/조회 함수. 파이프라인 단계 함수에서만 호출하고 직접 흐름을 시작하지 않는다.
// ============================================================================


FHexOffsetCoord ABattleManager::GetPlayerTargetingCoord() const
{
	const ABattleCharacterBase* TargetingActor = GetPlayerTargetingActor();
	return TargetingActor ? TargetingActor->GetCharacterCoord() : FHexOffsetCoord::Invalid();
}


FHexOffsetCoord ABattleManager::GetEnemyTargetingCoord() const
{
	const ABattleCharacterBase* TargetingActor = GetEnemyTargetingActor();
	return TargetingActor ? TargetingActor->GetCharacterCoord() : FHexOffsetCoord::Invalid();
}


ABattleCharacterBase* ABattleManager::GetPlayerTargetingActor() const
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


ABattleCharacterBase* ABattleManager::GetEnemyTargetingActor() const
{
	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		if (ABattleCharacterBase* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(EnemyBattleCharacter))
		{
			return SimulationCharacter;
		}
	}
	return EnemyBattleCharacter;
}


bool ABattleManager::ResolveActionTargetingForCurrentGrid(const FBattleAction& Action, FResolvedTargeting& OutResolvedTargeting) const
{
	FBattleAction RuntimeAction = Action;

	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		if (ABattleSimulationCharacter* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(Action.Attacker))
		{
			RuntimeAction.Attacker = SimulationCharacter;
		}
	}

	return FBattleTargetResolver::ResolveAction(RuntimeAction, BattleGridManager, OutResolvedTargeting);
}


bool ABattleManager::ResolveActionTargetingThroughStepForCurrentGrid(const FBattleAction& Action,
	int32 LastStepIndex,
	FResolvedTargeting& OutResolvedTargeting) const
{
	FBattleAction RuntimeAction = Action;

	if (BattleSimulationManager && BattleSimulationManager->IsSimulationRunning())
	{
		if (ABattleSimulationCharacter* SimulationCharacter = BattleSimulationManager->GetSimulationCharacter(Action.Attacker))
		{
			RuntimeAction.Attacker = SimulationCharacter;
		}
	}

	return FBattleTargetResolver::ResolveActionThroughStep(
		RuntimeAction,
		BattleGridManager,
		LastStepIndex,
		OutResolvedTargeting);
}


void ABattleManager::EndTargetingSessions()
{
	ClearSelectionAndRevealPreviews();
	PendingPlayerCard = nullptr;
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


UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Player(int32 ExchangeCount)
{
	if (!PlayerExchangeActions.IsValidIndex(ExchangeCount))
	{
		UE_LOG(LogTemp, Error, TEXT("Exchange Count is bigger then PlayerExchangeActions.Num (BattleManager.cpp)"));
		return nullptr;
	}
	UMuksiBattleCardDataAsset* Card = PlayerExchangeActions[ExchangeCount].Card;
	if (!Card)
	{
		UE_LOG(LogTemp, Error, TEXT("GetBattleCardDataAssetToExchange_Enemy is Null!!!"));
		return nullptr;
	}
	return Card;
}


UMuksiBattleCardDataAsset* ABattleManager::GetBattleCardDataAssetToExchange_Enemy(int32 ExchangeCount)
{
	if (!EnemyExchangeActions.IsValidIndex(ExchangeCount))
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyExchangeActions size : %d  Exchange Count %d"), EnemyExchangeActions.Num(), ExchangeCount);
		return nullptr;
	}
	UMuksiBattleCardDataAsset* Card = EnemyExchangeActions[ExchangeCount].Card;
	if (!Card)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]GetBattleCardDataAssetToExchange_Enemy is Null!!!"));
		return nullptr;
	}
	return Card;
}
