// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTriggerSubsystem.h"
#include"DialogueSubsystem.h"
#include"Algo/RandomShuffle.h"
#include"../ConditionHandle/CondTree/ConditionTreeEvaluator.h"
#include"DeveloperSettings/DialogueDeveloperSettings.h"

UDialogueTriggerSubsystem* UDialogueTriggerSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UDialogueTriggerSubsystem>() : nullptr;
}

void UDialogueTriggerSubsystem::InitializeSubsystem()
{
    //TODO:: For Test Initialization
    const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();

    const FDialogueTableEntry& Entry = Settings->DialogueTables[0];

    FName TableID = Entry.TableID;
    UDataTable* Table = Entry.Table.LoadSynchronous();

    FillUpTriggerKeys(TableID, Table);
}


void UDialogueTriggerSubsystem::ClearSingleTriggerIDs()
{
    SingleTriggerKeys.Empty();
    UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerSubsystem] SingleTriggerIDs cleared"));
}

void UDialogueTriggerSubsystem::ClearReusableTriggerIDs()
{
    ReusableTriggerKeys.Empty();

    UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerSubsystem] RepeatTriggerIDs cleared"));
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty(EDialogueTriggerType Type)
{
    TArray<FDialogueKey>* TriggerKeys = GetTrrigerArray(Type);

    return TriggerKeys->IsEmpty();
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty()
{
    return SingleTriggerKeys.IsEmpty() && ReusableTriggerKeys.IsEmpty() && ForTownTriggerKeys.IsEmpty();
}

void UDialogueTriggerSubsystem::FillUpTriggerKeys(FName TableID, UDataTable* DataTable)
{
    if (!DataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerSubsystem] DataTable is null"));
        return;
    }

    const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();

    for (const TPair<FName, uint8*>& Pair : RowMap)
    {
        const FName& RowName = Pair.Key;
        const FDialogueRow* Row = reinterpret_cast<FDialogueRow*>(Pair.Value);

        if (!Row)
            continue;

        if (!Row->meta.bIsEntry)
            continue;

        FDialogueKey Key;
        Key.TableID = TableID;
        Key.RowID = RowName;

        switch (Row->meta.TriggerType)
        {
        case EDialogueTriggerType::Single:
            SingleTriggerKeys.Add(Key);
            break;

        case EDialogueTriggerType::Reusable:
            ReusableTriggerKeys.Add(Key);
            break;

        case EDialogueTriggerType::ForTown:
            ForTownTriggerKeys.Add(Key);
            break;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerSubsystem] FillUpTriggerIDs Complete - Single: %d, Repeat: %d, ForTown: %d"),
        SingleTriggerKeys.Num(),
        ReusableTriggerKeys.Num(),
        ForTownTriggerKeys.Num());
}

FDialogueKey UDialogueTriggerSubsystem::ExtractRandomTriggerKey(EDialogueTriggerType Type)
{
    UDialogueSubsystem* DialogueSubsys = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    TArray<FDialogueKey>* TriggerKeys = GetTrrigerArray(Type);

    if (!TriggerKeys || TriggerKeys->IsEmpty())
        return FDialogueKey();

    TArray<int32> ValidIndices;

    for (int32 i = 0; i < TriggerKeys->Num(); ++i)
    {
        const FDialogueKey& Key = (*TriggerKeys)[i];

        UDataTable* Table = DialogueSubsys->GetTableByID(Key.TableID);
        if (!Table)
            continue;

        const FDialogueRow* Row = Table->FindRow<FDialogueRow>(Key.RowID, TEXT("TriggerCheck"));
        if (!Row)
            continue;

        if (FConditionTreeEvaluator::Evaluate(GetWorld(), Row->PopUpConditions))
        {
            ValidIndices.Add(i);
        }
    }

    if (ValidIndices.IsEmpty())
    {
        if (Type == EDialogueTriggerType::Reusable && !RecycleKeysBox.IsEmpty())
        {
            TriggerKeys->Append(RecycleKeysBox);
            RecycleKeysBox.Empty();

            return ExtractRandomTriggerKey(Type);
        }

        return FDialogueKey();
    }

    const int32 RandomIndex = ValidIndices[FMath::RandRange(0, ValidIndices.Num() - 1)];
    FDialogueKey Result = (*TriggerKeys)[RandomIndex];

    TriggerKeys->RemoveAtSwap(RandomIndex);

    if (Type == EDialogueTriggerType::Reusable)
    {
        RecycleKeysBox.Push(Result);

        if (TriggerKeys->IsEmpty())
        {
            TriggerKeys->Append(RecycleKeysBox);
            RecycleKeysBox.Empty();
        }
    }

    return Result;
}

void UDialogueTriggerSubsystem::RetrieveTriggerKey(const FDialogueKey& Key, EDialogueTriggerType Type)
{
    TArray<FDialogueKey>* TriggerKeys = GetTrrigerArray(Type);

    if (!TriggerKeys)
        return;

    if (!TriggerKeys->Contains(Key))
    {
        TriggerKeys->Add(Key);
    }

    if (Type == EDialogueTriggerType::Reusable)
    {
        RecycleKeysBox.Remove(Key);
    }
}

TArray<FDialogueKey>* UDialogueTriggerSubsystem::GetTrrigerArray(EDialogueTriggerType Type)
{
    switch (Type)
    {
    case EDialogueTriggerType::Single:
        return &SingleTriggerKeys;

    case EDialogueTriggerType::Reusable:
        return &ReusableTriggerKeys;

    case EDialogueTriggerType::ForTown:
        return &ForTownTriggerKeys;
    }

    return nullptr;
}