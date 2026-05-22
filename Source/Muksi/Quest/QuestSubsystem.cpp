#include "QuestSubsystem.h"

#include "Engine/DataTable.h"
#include "DeveloperSettings/QuestDeveloperSettings.h"
#include "QuestInstance_Base.h"
#include "QuestDetailRow.h"

UQuestSubsystem* UQuestSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
        return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();

    return GI ? GI->GetSubsystem<UQuestSubsystem>() : nullptr;
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitializeSubsystem();
}

void UQuestSubsystem::InitializeSubsystem()
{
    const UQuestDeveloperSettings* Settings = GetDefault<UQuestDeveloperSettings>();

    if (!Settings)
        return;

    LoadedTables.Empty();

    for (const FQuestTableEntry& Entry : Settings->QuestTables)
    {
        if (Entry.TableID.IsNone())
            continue;

        if (UDataTable* Table = Entry.Table.LoadSynchronous())
        {
            LoadedTables.Add( Entry.TableID, Table);

            UE_LOG( LogTemp, Warning, TEXT("[QuestSubsystem] Loaded Table : %s"), *Entry.TableID.ToString());
        }
    }
}

const FQuestDetailRow* UQuestSubsystem::GetQuestRow(const FQuestKey& QuestKey) const
{
    const TObjectPtr<UDataTable>* FoundTable = LoadedTables.Find(QuestKey.TableID);

    if (!FoundTable)
    {
        UE_LOG( LogTemp, Error, TEXT("[QuestSubsystem] Cannot Find TableID : %s"), *QuestKey.TableID.ToString());
        return nullptr;
    }

    return (*FoundTable)->FindRow<FQuestDetailRow>( QuestKey.RowID, TEXT("GetQuestRow"));
}

UQuestInstance_Base* UQuestSubsystem::AddNewQuest(const FQuestKey& QuestKey)
{
    if (!QuestKey.IsValid())
    {
        UE_LOG( LogTemp, Error, TEXT("[QuestSubsystem] Invalid QuestKey"));
        return nullptr;
    }

    if (ActiveQuests.Contains(QuestKey) || CompletedQuests.Contains(QuestKey))
    {
        UE_LOG( LogTemp, Error, TEXT("[QuestSubsystem] Already Has Quest : %s"), *QuestKey.ToString());

        return nullptr;
    }

    const FQuestDetailRow* QuestRow = GetQuestRow(QuestKey);

    if (!QuestRow)
    {
        UE_LOG( LogTemp, Error, TEXT("[QuestSubsystem] Cannot Find QuestRow : %s"), *QuestKey.ToString());

        return nullptr;
    }

    UQuestInstance_Base* NewQuest = NewObject<UQuestInstance_Base>(this);

    NewQuest->InitializeQuestInstance( QuestKey, *QuestRow, this);

    ActiveQuests.Add(QuestKey, NewQuest);

    UE_LOG( LogTemp, Warning, TEXT("[QuestSubsystem] Quest Accepted : %s"), *QuestKey.ToString());

    return NewQuest;
}

void UQuestSubsystem::CompleteQuest( const FQuestKey& QuestKey)
{
    UQuestInstance_Base* Quest = ActiveQuests.FindRef(QuestKey);

    if (!Quest)
        return;

    UE_LOG( LogTemp, Warning, TEXT("[QuestSubsystem] Quest Completed : %s"), *QuestKey.ToString());

    ActiveQuests.Remove(QuestKey);

    CompletedQuests.Add(QuestKey);

    OnQuestCompleted.Broadcast(Quest);
}

bool UQuestSubsystem::IsQuestActive( const FQuestKey& QuestKey) const
{
    return ActiveQuests.Contains(QuestKey);
}

bool UQuestSubsystem::IsQuestCompleted( const FQuestKey& QuestKey) const
{
    return CompletedQuests.Contains(QuestKey);
}

UQuestInstance_Base* UQuestSubsystem::GetQuestInstance(const FQuestKey& QuestKey) const
{
    return ActiveQuests.FindRef(QuestKey);
}

const TMap<FQuestKey, TObjectPtr<UQuestInstance_Base>>& UQuestSubsystem::GetActiveQuests() const
{
    return ActiveQuests;
}
