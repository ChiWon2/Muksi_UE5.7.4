// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTriggerSubsystem.h"
#include"DialogueSubsystem.h"
//#include"../Quest/QuestSubsystem.h"
#include"Algo/RandomShuffle.h"
#include"../ConditionHandle/GameConditionEvaluator.h"
#include"../ConditionHandle/CondTree/ConditionTreeEvaluator.h"

UDialogueTriggerSubsystem* UDialogueTriggerSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UDialogueTriggerSubsystem>() : nullptr;
}

void UDialogueTriggerSubsystem::FillUpTriggerIDs(UDataTable* DataTable)
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

        // Entry Dialogue¸¸ Trigger ´ë»ó
        if (!Row->meta.bIsEntry)
            continue;

        if (Row->meta.TriggerType == EDialogueTriggerType::Single)
            SingleTriggerIDs.Add(RowName);
        else if (Row->meta.TriggerType == EDialogueTriggerType::Reusable)
            ReusableTriggerIDs.Add(RowName);
        else if (Row->meta.TriggerType == EDialogueTriggerType::ForTown)
            ForTownTriggerIDs.Add(RowName);

    }
    UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerSubsystem] FillUpTriggerIDs Complete - Single: %d, Repeat: %d, ForTown: %d"), SingleTriggerIDs.Num(), ReusableTriggerIDs.Num(), ForTownTriggerIDs.Num());
}

void UDialogueTriggerSubsystem::ClearSingleTriggerIDs()
{
    SingleTriggerIDs.Empty();
    UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerSubsystem] SingleTriggerIDs cleared"));
}

void UDialogueTriggerSubsystem::ClearReusableTriggerIDs()
{
    ReusableTriggerIDs.Empty();

    UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerSubsystem] RepeatTriggerIDs cleared"));
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty(EDialogueTriggerType Type)
{
    TArray<FName>* TriggerIDs = GetTrrigerArray(Type);

    return TriggerIDs->IsEmpty();
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty()
{
    return SingleTriggerIDs.IsEmpty() && ReusableTriggerIDs.IsEmpty() && ForTownTriggerIDs.IsEmpty();
}

FName UDialogueTriggerSubsystem::ExtractRandomTriggerID(EDialogueTriggerType Type)
{
    UDialogueSubsystem* DialogueSubsys = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    TArray<FName>* TriggerIDs = GetTrrigerArray(Type);

    if (TriggerIDs->IsEmpty())
        return NAME_None;

    TArray<int32> ValidIndices;

    for (int i = 0; i < TriggerIDs->Num(); ++i)
    {
        const FName& ID = (*TriggerIDs)[i];
        const FDialogueRow* Row = DialogueSubsys->GetDialogueRow(ID);

        if (!Row)
            continue;

        if (FConditionTreeEvaluator::Evaluate(GetWorld(), Row->PopUpConditions))
        {
            ValidIndices.Add(i);
        }
    }

    if (ValidIndices.IsEmpty())
    {
        if (Type == EDialogueTriggerType::Reusable && !RecycleIDsBox.IsEmpty())
        {
            TriggerIDs->Append(RecycleIDsBox);
            RecycleIDsBox.Empty();

            return ExtractRandomTriggerID(Type);
        }

        return NAME_None;
    }

    const int32 RandomIndex = ValidIndices[FMath::RandRange(0, ValidIndices.Num() - 1)];

    FName Result = (*TriggerIDs)[RandomIndex];

    TriggerIDs->RemoveAtSwap(RandomIndex);

    if (Type == EDialogueTriggerType::Reusable)
    {
        RecycleIDsBox.Push(Result);
        if (TriggerIDs->IsEmpty())
        {
            TriggerIDs->Append(RecycleIDsBox);
            RecycleIDsBox.Empty();
        }
    }

    return Result;
}
void UDialogueTriggerSubsystem::RetrieveTriggerID(const FName& ID, EDialogueTriggerType Type)
{
    switch (Type)
    {
    case EDialogueTriggerType::Single:
    {
        if(!SingleTriggerIDs.Contains(ID))
            SingleTriggerIDs.Add(ID);
    }
        break;
    case EDialogueTriggerType::Reusable:
    {
        if (!ReusableTriggerIDs.Contains(ID))
            ReusableTriggerIDs.Add(ID);

        RecycleIDsBox.Remove(ID);
    }
        break;
    case EDialogueTriggerType::ForTown:
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueTrrigerSubSystem] : there is no RetrieveFunction for town \"YET\""));
        return;
    }
        break;
    default:
        return;
    }
}

TArray<FName>* UDialogueTriggerSubsystem::GetTrrigerArray(EDialogueTriggerType Type)
{
    switch (Type)
    {
    case EDialogueTriggerType::Single:
        return &SingleTriggerIDs;
        break;
    case EDialogueTriggerType::Reusable:
        return &ReusableTriggerIDs;
        break;
    case EDialogueTriggerType::ForTown:
        return &ForTownTriggerIDs;
        break;
    }
    return nullptr;
}