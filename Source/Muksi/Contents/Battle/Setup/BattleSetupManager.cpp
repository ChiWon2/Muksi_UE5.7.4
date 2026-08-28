#include "Muksi/Contents/Battle/Setup/BattleSetupManager.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"

ABattleSetupManager::ABattleSetupManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleSetupManager::BeginPlay()
{
    Super::BeginPlay();
}

void ABattleSetupManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindBattleEndEvents();
    if (BattleManager)
        BattleManager->PhaseEntryRequestedDelegate.RemoveDynamic(this, &ABattleSetupManager::HandlePhaseEntryRequested);

    BattleGridManager = nullptr;
    BattleManager = nullptr;

    Super::EndPlay(EndPlayReason);
}

bool ABattleSetupManager::InitializeBattleFlow(ABattleManager* InBattleManager, ABattleGridManager* InBattleGridManager)
{
    if (!IsValid(InBattleManager) || !IsValid(InBattleGridManager))
        return false;

    BattleManager = InBattleManager;
    BattleGridManager = InBattleGridManager;
    BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &ABattleSetupManager::HandlePhaseEntryRequested);
    return true;
}

bool ABattleSetupManager::ShouldHandlePhaseEntry(EBattlePhase Phase) const
{
    return Phase == EBattlePhase::ReadyStart || Phase == EBattlePhase::ReadyEnd;
}

void ABattleSetupManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)OldPhase;

    if (!ShouldHandlePhaseEntry(NewPhase) || !TaskContext)
        return;

    UBattlePhaseTask* Task = TaskContext->RegisterTask(this);
    if (!Task)
        return;

    if (!IsValid(BattleManager) || !IsValid(BattleGridManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleSetupManager] Failed to resolve phase entry dependencies."));
        Task->Complete();
        return;
    }

    switch (NewPhase)
    {
    case EBattlePhase::ReadyStart:
        PrepareReadyData();
        break;

    case EBattlePhase::ReadyEnd:
        PrepareReadyEnd();
        break;

    default:
        break;
    }

    Task->Complete();
}

bool ABattleSetupManager::PrepareReadyData()
{
    LoadEncounterEnemyCharacterData();

    if (!IsValid(PlayerCharacterDataAsset))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleSetupManager] Player character data is invalid."));
        return false;
    }

    if (!IsValid(EnemyCharacterDataAsset))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleSetupManager] Enemy character data is invalid."));
        return false;
    }

    return true;
}

bool ABattleSetupManager::PrepareReadyEnd()
{
    if (!CreateBattleCharacters())
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleSetupManager] Failed to create battle characters."));
        return false;
    }

    BindBattleEndEvents();
    return true;
}

void ABattleSetupManager::LoadEncounterEnemyCharacterData()
{
    UBattleEncounterSubsystem* BattleEncounterSubsystem = UBattleEncounterSubsystem::Get(this);
    if (!IsValid(BattleEncounterSubsystem))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BattleSetupManager] BattleEncounterSubsystem is invalid. Using configured enemy data."));
        return;
    }

    UMuksiCharacterDataAsset* EncounterEnemyData = BattleEncounterSubsystem->GetCurrentEnemyData();
    if (!IsValid(EncounterEnemyData))
    {
        UE_LOG(LogTemp, Warning, TEXT("[BattleSetupManager] Current enemy data is invalid. Using configured enemy data."));
        return;
    }

    EnemyCharacterDataAsset = EncounterEnemyData;
}

bool ABattleSetupManager::CreateBattleCharacters()
{
    if (!IsValid(BattleManager) || !IsValid(BattleGridManager))
    {
        return false;
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (!IsValid(RuntimeContext))
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World || !IsValid(PlayerCharacterDataAsset) || !IsValid(EnemyCharacterDataAsset))
    {
        return false;
    }

    TSubclassOf<ABattleCharacterBase> PlayerClass = PlayerCharacterDataAsset->BattleCharacterClass;
    TSubclassOf<ABattleCharacterBase> EnemyClass = EnemyCharacterDataAsset->BattleCharacterClass;
    if (!PlayerClass || !EnemyClass)
    {
        return false;
    }

    UnbindBattleEndEvents();

    if (ABattleCharacter_Player* ExistingPlayer = RuntimeContext->GetPlayerCharacter())
    {
        ExistingPlayer->Destroy();
    }

    if (ABattleCharacter_Enemy* ExistingEnemy = RuntimeContext->GetEnemyCharacter())
    {
        ExistingEnemy->Destroy();
    }

    RuntimeContext->SetBattleCharacters(nullptr, nullptr);

    const FTransform SpawnTransform = BattleManager->GetActorTransform();
    ABattleCharacter_Player* PlayerCharacter = World->SpawnActor<ABattleCharacter_Player>(PlayerClass, SpawnTransform);
    if (!IsValid(PlayerCharacter))
    {
        return false;
    }

    PlayerCharacter->SetCharacterData(PlayerCharacterDataAsset, BattleManager);

    ABattleCharacter_Enemy* EnemyCharacter = World->SpawnActor<ABattleCharacter_Enemy>(EnemyClass, SpawnTransform);
    if (!IsValid(EnemyCharacter))
    {
        PlayerCharacter->Destroy();
        return false;
    }

    EnemyCharacter->SetCharacterData(EnemyCharacterDataAsset, BattleManager);

    BattleGridManager->PlaceCharacter(EBattleSimulationWorldType::PlayerActualEnemyActual, PlayerCharacter, StartPlayerCoord);
    BattleGridManager->PlaceCharacter(EBattleSimulationWorldType::PlayerActualEnemyActual, EnemyCharacter, StartEnemyCoord);
    RuntimeContext->SetBattleCharacters(PlayerCharacter, EnemyCharacter);
    return true;
}

void ABattleSetupManager::BindBattleEndEvents()
{
    UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
    if (!IsValid(RuntimeContext))
    {
        return;
    }

    ABattleCharacter_Player* PlayerCharacter = RuntimeContext->GetPlayerCharacter();
    ABattleCharacter_Enemy* EnemyCharacter = RuntimeContext->GetEnemyCharacter();

    if (!IsValid(PlayerCharacter) || !IsValid(EnemyCharacter))
    {
        return;
    }

    if (UBattleStatComponent* PlayerStatComponent = PlayerCharacter->GetBattleStatComponent())
    {
        PlayerStatComponent->OnDead.RemoveDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
        PlayerStatComponent->OnDead.AddUniqueDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
    }

    if (UBattleStatComponent* EnemyStatComponent = EnemyCharacter->GetBattleStatComponent())
    {
        EnemyStatComponent->OnDead.RemoveDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
        EnemyStatComponent->OnDead.AddUniqueDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
    }
}

void ABattleSetupManager::UnbindBattleEndEvents()
{
    UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
    if (!IsValid(RuntimeContext))
    {
        return;
    }

    if (ABattleCharacter_Player* PlayerCharacter = RuntimeContext->GetPlayerCharacter())
    {
        if (UBattleStatComponent* PlayerStatComponent = PlayerCharacter->GetBattleStatComponent())
        {
            PlayerStatComponent->OnDead.RemoveDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
        }
    }

    if (ABattleCharacter_Enemy* EnemyCharacter = RuntimeContext->GetEnemyCharacter())
    {
        if (UBattleStatComponent* EnemyStatComponent = EnemyCharacter->GetBattleStatComponent())
        {
            EnemyStatComponent->OnDead.RemoveDynamic(this, &ABattleSetupManager::HandleBattleCharacterDead);
        }
    }
}

void ABattleSetupManager::HandleBattleCharacterDead(ABattleCharacterBase* DeadCharacter)
{
    (void)DeadCharacter;

    if (BattleManager)
    {
        BattleManager->NotifyBattleCharacterDead();
    }
}
