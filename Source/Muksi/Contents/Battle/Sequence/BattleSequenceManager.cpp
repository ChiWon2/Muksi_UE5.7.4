#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Sequence/Environment/BattleSequenceExecutionEnvironment.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

// ============================================================================
// 생명주기
// ============================================================================
ABattleSequenceManager::ABattleSequenceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleSequenceManager::BeginPlay()
{
	Super::BeginPlay();

	if (!bWorldManagerRegistrationEnabled)
	{
		return;
	}

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleSequenceManager>(this);
	}
}

void ABattleSequenceManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bWorldManagerRegistrationEnabled)
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
		{
			ManagerSubsystem->UnregisterManager<ABattleSequenceManager>(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

// ============================================================================
// Sequence 진입
// BattleManager 또는 BattleSimulationManager -> StartSequence(WithRequest)
// ============================================================================
bool ABattleSequenceManager::StartSequence(const FBattleAction& InAction)
{
	FBattleSequenceRequest Request;
	Request.Action = InAction;
	Request.ExecutionMode = EBattleExecutionMode::Sequence;
	return StartSequenceWithRequest(Request);
}

bool ABattleSequenceManager::StartSequenceWithRequest(const FBattleSequenceRequest& Request)
{
	if (bSequenceRunning || !ValidateRequest(Request))
	{
		return false;
	}

	FBattleAction SequenceAction = Request.Action;
	SequenceAction.Card = Request.GetExecutionCard();

	if (!FBattleTargetResolver::ResolveAction(SequenceAction, BattleGridManager, CurrentResolvedTargeting))
	{
		return false;
	}

	CurrentAction = MoveTemp(SequenceAction);
	CurrentExecutionMode = Request.ExecutionMode;
	bSequenceRunning = true;
	ActiveExecutionRunners.Empty();

	if (!InitializeExecutionEnvironment())
	{
		FinishSequence();
		return false;
	}

	if (!BindAttackerNotify())
	{
		FinishSequence();
		return false;
	}

	StartMainExecutionChain();
	return true;
}

// ============================================================================
// 요청 검증 및 Execution 환경 준비
// ============================================================================
bool ABattleSequenceManager::ValidateRequest(const FBattleSequenceRequest& Request) const
{
	UMuksiBattleCardDataAsset* ExecutionCard = Request.GetExecutionCard();
	return IsValid(Request.Action.Attacker) && IsValid(ExecutionCard) && !ExecutionCard->MainExecutions.IsEmpty();
}

bool ABattleSequenceManager::InitializeExecutionEnvironment()
{
	ExecutionEnvironment = NewObject<UBattleSequenceExecutionEnvironment>(this);

	if (!ExecutionEnvironment)
	{
		return false;
	}

	ExecutionEnvironment->InitializeSequence(CurrentAction.Attacker, CurrentAction.Card, BattleGridManager);
	return ExecutionEnvironment->IsValidEnvironment();
}

bool ABattleSequenceManager::BindAttackerNotify()
{
	if (!CurrentAction.Card || CurrentAction.Card->NotifyExecutionChains.IsEmpty())
	{
		return true;
	}

	if (!CurrentAction.Attacker)
	{
		return false;
	}

	AttackerAnimationComponent = CurrentAction.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AttackerAnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] AnimationComponent not found. Attacker=%s"), *GetNameSafe(CurrentAction.Attacker.Get()));
		return false;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.AddUniqueDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
	return true;
}

void ABattleSequenceManager::UnbindAttackerNotify()
{
	if (!AttackerAnimationComponent)
	{
		return;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.RemoveDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
}

// ============================================================================
// Execution Chain 실행
// Main chain + Animation Notify chain + Runtime requested chain
// ============================================================================
void ABattleSequenceManager::StartMainExecutionChain()
{
	if (!CurrentAction.Card || CurrentAction.Card->MainExecutions.IsEmpty())
	{
		FinishSequence();
		return;
	}

	StartExecutionRunner(CurrentAction.Card->MainExecutions, MakeExecutionContext(NAME_None));
}

void ABattleSequenceManager::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (!bSequenceRunning || NotifyKey.IsNone())
	{
		return;
	}

	StartNotifyExecutionChains(NotifyKey);
}

void ABattleSequenceManager::StartNotifyExecutionChains(FName NotifyKey)
{
	if (!CurrentAction.Card)
	{
		return;
	}

	for (const FBattleNotifyExecutionChain& NotifyChain : CurrentAction.Card->NotifyExecutionChains)
	{
		if (!NotifyChain.IsValid() || NotifyChain.NotifyKey != NotifyKey)
		{
			continue;
		}

		StartExecutionRunner(NotifyChain.Executions, MakeExecutionContext(NotifyKey));
	}
}

void ABattleSequenceManager::StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	if (!bSequenceRunning || ExecutionEntries.IsEmpty())
	{
		return;
	}

	UBattleExecutionRunner* ExecutionRunner = NewObject<UBattleExecutionRunner>(this);

	if (!ExecutionRunner)
	{
		return;
	}

	ActiveExecutionRunners.Add(ExecutionRunner);

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionRunner Started. Runner=%s ActiveRunners=%d"), *GetNameSafe(ExecutionRunner), ActiveExecutionRunners.Num());

	FBattleExecutionEntryStarted OnEntryStarted;
	OnEntryStarted.BindUObject(this, &ABattleSequenceManager::HandleExecutionEntryStarted);

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionRunnerFinished);

	ExecutionRunner->Run(ExecutionEntries, Context, OnEntryStarted, OnFinished);
}

void ABattleSequenceManager::HandleRuntimeExecutionChainRequested(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	StartExecutionRunner(ExecutionEntries, Context);
}

FBattleExecutionContext ABattleSequenceManager::MakeExecutionContext(FName NotifyKey)
{
	FBattleExecutionContext Context;

	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentAction.Card;
	Context.ExecutionMode = CurrentExecutionMode;
	Context.Environment = ExecutionEnvironment;
	Context.ResolvedTargeting = CurrentResolvedTargeting;
	Context.BattleGridManager = BattleGridManager;
	Context.NotifyKey = NotifyKey;
	Context.RequestRuntimeExecutionChain.BindUObject(this, &ABattleSequenceManager::HandleRuntimeExecutionChainRequested);

	return Context;
}

void ABattleSequenceManager::HandleExecutionEntryStarted(
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	FBattleExecutionContext& InOutExecutionContext)
{
	if (!bSequenceRunning)
	{
		return;
	}

	// Targeting intent is preserved for the whole sequence, while the resolved
	// result is rebuilt immediately before every execution against the current grid.
	// Preview and execution therefore consume the same snapshot.
	FResolvedTargeting RefreshedTargeting;
	if (FBattleTargetResolver::ResolveAction(CurrentAction, BattleGridManager, RefreshedTargeting))
	{
		CurrentResolvedTargeting = MoveTemp(RefreshedTargeting);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattleSequenceManager] Targeting refresh failed before execution. EntryIndex=%d Execution=%s"),
			EntryIndex,
			*GetNameSafe(Entry.ExecutionClass.Get()));
		CurrentResolvedTargeting.Reset();
	}

	InOutExecutionContext.ResolvedTargeting = CurrentResolvedTargeting;
	OnExecutionEntryStarted.Broadcast(CurrentAction, Entry, EntryIndex, CurrentResolvedTargeting);
}

void ABattleSequenceManager::HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bSequenceRunning || !FinishedRunner)
	{
		return;
	}

	const int32 RemovedCount = ActiveExecutionRunners.RemoveSingle(FinishedRunner);

	if (RemovedCount == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionRunner Finished. Runner=%s ActiveRunners=%d"), *GetNameSafe(FinishedRunner), ActiveExecutionRunners.Num());

	TryFinishSequence();
}

// ============================================================================
// Sequence 종료
// 모든 Runner 완료 -> FinishSequence -> OnSequenceFinished
// ============================================================================
void ABattleSequenceManager::TryFinishSequence()
{
	if (!bSequenceRunning || !ActiveExecutionRunners.IsEmpty())
	{
		return;
	}

	FinishSequence();
}

void ABattleSequenceManager::FinishSequence()
{
	if (!bSequenceRunning)
	{
		return;
	}

	UnbindAttackerNotify();

	bSequenceRunning = false;
	CurrentAction = FBattleAction();
	CurrentResolvedTargeting.Reset();
	CurrentExecutionMode = EBattleExecutionMode::Sequence;
	AttackerAnimationComponent = nullptr;
	ActiveExecutionRunners.Empty();
	ExecutionEnvironment = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Sequence Finished."));

	OnSequenceFinished.Broadcast();
}
