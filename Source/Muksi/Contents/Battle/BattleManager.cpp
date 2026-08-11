#include "Muksi/Contents/Battle/BattleManager.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Round/BattleRoundPhaseCoordinator.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"


ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	BattleRuntimeContext = NewObject<UBattleRuntimeContext>(this);
	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->ResetBattle();
		RoundPhaseCoordinator = NewObject<UBattleRoundPhaseCoordinator>(this);
		if (!RoundPhaseCoordinator || !RoundPhaseCoordinator->Initialize(this))
		{
			UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to initialize BattleRoundPhaseCoordinator."));
			RoundPhaseCoordinator = nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to create BattleRuntimeContext."));
	}

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleManager>(this);
	}
}


void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RoundPhaseCoordinator)
	{
		RoundPhaseCoordinator->Shutdown();
		RoundPhaseCoordinator = nullptr;
	}

	BattleRuntimeContext = nullptr;

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->UnregisterManager<ABattleManager>(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABattleManager::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
	BattleActionStartDelegate.Broadcast(BattleAction);
}

void ABattleManager::ReadyStart()
{
	bIsCharacterDead = false;
	ChangePhase(EBattlePhase::ReadyStart);
}

void ABattleManager::ReadyEnd()
{
	ChangePhase(EBattlePhase::ReadyEnd);
}

void ABattleManager::BattleStart()
{
	CurrentRound = 0;
	CurrentExchange = 0;
	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->ResetRound(CurrentRound);
	}
	ChangePhase(EBattlePhase::BattleStart);
	NotifyPhaseExecutionFinished();
}


void ABattleManager::BattleEnd()
{
	if (CurrentPhase == EBattlePhase::BattleEnd)
	{
		return;
	}

	ChangePhase(EBattlePhase::BattleEnd);
	NotifyPhaseExecutionFinished();
}
void ABattleManager::RoundStart()
{
	++CurrentRound;
	CurrentExchange = 0;
	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->ResetRound(CurrentRound);
	}
	ChangePhase(EBattlePhase::RoundStart);
	if (!RoundPhaseCoordinator)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] RoundPhaseCoordinator is unavailable. Skipping round start processing."));
		NotifyPhaseExecutionFinished();
	}
}

void ABattleManager::NotifyBattleCharacterDead()
{
	if (bIsCharacterDead)
	{
		return;
	}

	bIsCharacterDead = true;
	BattleEnd();
}

void ABattleManager::EndBattleLevel()
{
	UBattleEncounterSubsystem* EncounterSubsystem = UBattleEncounterSubsystem::Get(this);
	if (!IsValid(EncounterSubsystem))
	{
		return;
	}

	FBattleResult BattleResult;
	// TODO: 전투 종료 시 체력, 경험치 등 최종 전투 결과를 기록한다.
	EncounterSubsystem->FinishBattleEncounter(BattleResult);
}

void ABattleManager::RoundEnd()
{
	ChangePhase(EBattlePhase::RoundEnd);
	if (!RoundPhaseCoordinator)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] RoundPhaseCoordinator is unavailable. Skipping round end processing."));
		NotifyPhaseExecutionFinished();
	}
}


bool ABattleManager::ShouldEndBattle() const
{
	return bIsCharacterDead;
}


void ABattleManager::ExchangeStart()
{
	if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid exchange start index: %d"), CurrentExchange);
		return;
	}

	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->SetCurrentExchange(CurrentExchange);
	}

	ChangePhase(EBattlePhase::ExchangeStart);
	NotifyPhaseExecutionFinished();
}


void ABattleManager::StartExchangeSelectCard()
{
	if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid current exchange: %d"), CurrentExchange);
		return;
	}

	ChangePhase(EBattlePhase::CardSelect);
	RequestPhaseUI();
}


void ABattleManager::AdvanceExchange()
{
	if (CurrentExchange + 1 < MaxExchangeCount)
	{
		++CurrentExchange;
		ExchangeStart();
		return;
	}
	BattleActionSequenceStart();
}

void ABattleManager::ExchangeEnd()
{
	ChangePhase(EBattlePhase::ExchangeEnd);
	NotifyPhaseExecutionFinished();
}


void ABattleManager::BattleActionSequenceStart()
{
	if (!BattleRuntimeContext || !BattleRuntimeContext->HasBattleActionSequenceActions())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Battle Action Sequence에 실행할 행동이 없습니다."));
		BattleActionSequenceEnd();
		return;
	}

	ChangePhase(EBattlePhase::BattleActionSequenceStart);
	NotifyPhaseExecutionFinished();
}

void ABattleManager::BattleActionSequenceEnd()
{
	if (CurrentPhase == EBattlePhase::BattleActionSequenceEnd)
	{
		return;
	}

	ChangePhase(EBattlePhase::BattleActionSequenceEnd);
	NotifyPhaseExecutionFinished();
}

void ABattleManager::NotifyPhaseExecutionFinished()
{
	const EBattlePhase FinishedPhase = CurrentPhase;

	// UI 완료 뒤 시작되는 외부 실행이 끝난 경우에는 UI를 다시 요청하지 않는다.
	if (bCurrentPhaseUIFinished)
	{
		AdvanceFromPhase(FinishedPhase);
		return;
	}

	const bool bRequestedUI = RequestPhaseUI();
	if (!bRequestedUI)
	{
		bCurrentPhaseUIFinished = true;
		PhaseUIFinishedDelegate.Broadcast(PreviousPhase, CurrentPhase);

		if (!ShouldWaitForExternalExecutionAfterUI(FinishedPhase))
		{
			AdvanceFromPhase(FinishedPhase);
		}
		return;
	}

	if (!ShouldWaitForPhaseUI(FinishedPhase))
	{
		AdvanceFromPhase(FinishedPhase);
	}
}


void ABattleManager::NotifyPhaseUIFinished(EBattlePhase FinishedPhase)
{
	if (CurrentPhase != FinishedPhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Ignored stale phase UI completion. Current: %d, Finished: %d"), static_cast<int32>(CurrentPhase), static_cast<int32>(FinishedPhase));
		return;
	}

	bCurrentPhaseUIFinished = true;
	PhaseUIFinishedDelegate.Broadcast(PreviousPhase, CurrentPhase);

	if (!ShouldWaitForExternalExecutionAfterUI(FinishedPhase))
	{
		AdvanceFromPhase(FinishedPhase);
	}
}

void ABattleManager::NotifyInteractivePhaseFinished(EBattlePhase FinishedPhase)
{
	if (CurrentPhase != FinishedPhase)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattleManager] Ignored stale interactive phase completion. Current: %d, Finished: %d"),
			static_cast<int32>(CurrentPhase),
			static_cast<int32>(FinishedPhase));
		return;
	}

	AdvanceFromPhase(FinishedPhase);
}

void ABattleManager::RestartCurrentExchangeCardSelection()
{
	if (CurrentPhase != EBattlePhase::Targeting && CurrentPhase != EBattlePhase::CardReveal)
	{
		return;
	}

	ChangePhase(EBattlePhase::CardSelect);
	RequestPhaseUI();
}


void ABattleManager::AdvanceFromPhase(EBattlePhase FinishedPhase)
{
	switch (FinishedPhase)
	{
	case EBattlePhase::ReadyStart:
		ReadyEnd();
		break;

	case EBattlePhase::ReadyEnd:
		BattleStart();
		break;

	case EBattlePhase::BattleStart:
		RoundStart();
		break;

	case EBattlePhase::RoundStart:
		ExchangeStart();
		break;

	case EBattlePhase::ExchangeStart:
		StartExchangeSelectCard();
		break;

	case EBattlePhase::CardSelect:
		ChangePhase(EBattlePhase::Targeting);
		break;

	case EBattlePhase::Targeting:
		ChangePhase(EBattlePhase::CardReveal);
		if (!RequestPhaseUI())
		{
			AdvanceFromPhase(EBattlePhase::CardReveal);
		}
		break;

	case EBattlePhase::CardReveal:
		ChangePhase(EBattlePhase::SimulationSequence);
		break;

	case EBattlePhase::SimulationSequence:
		ExchangeEnd();
		break;

	case EBattlePhase::ExchangeEnd:
		AdvanceExchange();
		break;

	case EBattlePhase::BattleActionSequenceStart:
		BattleActionSequenceEnd();
		break;

	case EBattlePhase::BattleActionSequenceEnd:
		RoundEnd();
		break;

	case EBattlePhase::RoundEnd:
		if (ShouldEndBattle())
		{
			BattleEnd();
		}
		else
		{
			RoundStart();
		}
		break;

	case EBattlePhase::BattleEnd:
		EndBattleLevel();
		break;

	default:
		break;
	}
}


bool ABattleManager::ShouldWaitForPhaseUI(EBattlePhase Phase) const
{
	switch (Phase)
	{
	case EBattlePhase::ReadyStart:
	case EBattlePhase::ReadyEnd:
	case EBattlePhase::SimulationSequence:
		return false;

	default:
		return true;
	}
}


bool ABattleManager::ShouldWaitForExternalExecutionAfterUI(EBattlePhase Phase) const
{
	return Phase == EBattlePhase::RoundStart || Phase == EBattlePhase::BattleActionSequenceStart;
}


bool ABattleManager::RequestPhaseUI()
{
	if (!PhaseUIDelegate.IsBound())
	{
		return false;
	}

	PhaseUIDelegate.Broadcast(PreviousPhase, CurrentPhase);
	return true;
}



void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager]: You Try to Change Same Battle Phase!"));
		return;
	}

	PreviousPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	bCurrentPhaseUIFinished = false;

	ChangePhaseDelegate.Broadcast(PreviousPhase, CurrentPhase);
}


