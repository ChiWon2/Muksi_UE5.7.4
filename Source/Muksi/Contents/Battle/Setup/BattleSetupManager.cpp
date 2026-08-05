#include "Muksi/Contents/Battle/Setup/BattleSetupManager.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"

ABattleSetupManager::ABattleSetupManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleSetupManager::BeginPlay()
{
    Super::BeginPlay();

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
    {
        ManagerSubsystem->RegisterManager<ABattleSetupManager>(this);
    }

    if (!TryBindBattleFlow())
    {
        BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ABattleSetupManager::BindBattleFlowDeferred));
    }
}

void ABattleSetupManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(BattleFlowBindingTimerHandle);

    if (BattleManager)
    {
        BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSetupManager::HandleBattlePhaseChanged);
    }

    UnbindBattleEndEvents();
    BattleGridManager = nullptr;
    BattleManager = nullptr;

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
    {
        ManagerSubsystem->UnregisterManager<ABattleSetupManager>(this);
    }

    Super::EndPlay(EndPlayReason);
}

bool ABattleSetupManager::TryBindBattleFlow()
{
    UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
    if (!ManagerSubsystem)
    {
        return false;
    }

    ABattleManager* FoundBattleManager = ManagerSubsystem->GetManager<ABattleManager>();
    if (!IsValid(FoundBattleManager) || !IsValid(FoundBattleManager->GetBattleRuntimeContext()))
    {
        return false;
    }

    ABattleGridManager* FoundGridManager = BattleGridManager;
    if (!IsValid(FoundGridManager))
    {
        FoundGridManager = ManagerSubsystem->GetManager<ABattleGridManager>();
    }

    if (!IsValid(FoundGridManager))
    {
        return false;
    }

    if (BattleManager && BattleManager != FoundBattleManager)
    {
        BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSetupManager::HandleBattlePhaseChanged);
    }

    BattleManager = FoundBattleManager;
    BattleGridManager = FoundGridManager;

    BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSetupManager::HandleBattlePhaseChanged);
    BattleManager->ChangePhaseDelegate.AddUniqueDynamic(this, &ABattleSetupManager::HandleBattlePhaseChanged);

    const EBattlePhase CurrentPhase = BattleManager->GetCurrentPhase();
    if (CurrentPhase == EBattlePhase::Ready)
    {
        HandleBattlePhaseChanged(EBattlePhase::None, CurrentPhase);
    }
    else if (CurrentPhase == EBattlePhase::ReadyEnd)
    {
        HandleBattlePhaseChanged(EBattlePhase::Ready, CurrentPhase);
    }

    return true;
}

void ABattleSetupManager::BindBattleFlowDeferred()
{
    if (TryBindBattleFlow())
    {
        return;
    }

    BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateUObject(this, &ABattleSetupManager::BindBattleFlowDeferred));
}

void ABattleSetupManager::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    (void)OldPhase;

    switch (NewPhase)
    {
    case EBattlePhase::Ready:
        if (PrepareReadyData())
        {
            BattleManager->NotifyPhaseExecutionFinished();
        }
        break;

    case EBattlePhase::ReadyEnd:
        if (PrepareReadyEnd())
        {
            BattleManager->NotifyPhaseExecutionFinished();
        }
        break;

    default:
        break;
    }
}

bool ABattleSetupManager::PrepareReadyData()
{
    ResolveEnemyCharacterData();

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

void ABattleSetupManager::ResolveEnemyCharacterData()
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

    BattleGridManager->PlaceCharacter(PlayerCharacter, StartPlayerCoord);
    BattleGridManager->PlaceCharacter(EnemyCharacter, StartEnemyCoord);
    RuntimeContext->SetBattleCharacters(PlayerCharacter, EnemyCharacter);
    return true;
}

void ABattleSetupManager::BindBattleEndEvents()
{
    UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager)
        ? BattleManager->GetBattleRuntimeContext()
        : nullptr;
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
    UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager)
        ? BattleManager->GetBattleRuntimeContext()
        : nullptr;
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
