// Fill out your copyright notice in the Description page of Project Settings.
#include "EventHandleSubsystem.h"
#include"Events/AddItemEvent.h"
#include"Events/AddStatEvent.h"
#include"Events/AcceptQuestEvent.h"
#include"Events/CompleteQuestEvent.h"
#include"Events/FailQuestEvent.h"
#include"Events/StartBattleEvent.h"
#include"Events/CreateQuestObjectiveEvent.h"
#include"Events/ChangeDialogueContextEvent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UEventHandleSubsystem* UEventHandleSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UEventHandleSubsystem>() : nullptr;
}

void UEventHandleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    RegisterDefaultEventObjects();
}

void UEventHandleSubsystem::Deinitialize()
{
    ExportEventsToCSV();
}

void UEventHandleSubsystem::RegisterEventObject(UBaseEvent* Event)
{
    if (Event && !EventMap.Contains(Event->EventID))
    {
        EventMap.Add(Event->EventID, Event);
        UE_LOG(LogTemp, Log, TEXT("[EventHandleSubsystem] Registered Event: %s"), *Event->EventID.ToString());
    }
}

//TODO:: Delete this code when you publish the game
void UEventHandleSubsystem::ExportEventsToCSV()
{
    FString CSVContent = TEXT("Event,Description\n");

    for (const auto& EventPair : EventMap)
    {
        FString EventName = EventPair.Key.ToString();
        FString Description = EventPair.Value->EventDescribe.ToString();

        Description.ReplaceInline(TEXT("\""), TEXT("\"\""));

        CSVContent += FString::Printf(TEXT("\"%s\",\"%s\"\n"), *EventName, *Description);
    }

    FString SavePath = FPaths::ProjectSavedDir() / TEXT("EventList.csv");

    if (FFileHelper::SaveStringToFile(CSVContent,*SavePath,FFileHelper::EEncodingOptions::ForceUTF8))
    {
        UE_LOG(LogTemp, Warning, TEXT("[EventHandleSubsystem] CSV Export Success: %s You can search the eventTag and Description in this File"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[CSV Export] Failed: %s"), *SavePath);
    }
}

void UEventHandleSubsystem::ExecuteEvent(FName EventID, const FEventParameter& Param)
{
    if (UBaseEvent** Found = EventMap.Find(EventID))
    {
        (*Found)->Execute(Param);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Event %s not found!"), *EventID.ToString());
    }
}

void UEventHandleSubsystem::RegisterDefaultEventObjects()
{
    RegisterEvent<UAcceptQuestEvent>("AcceptQuest", FText::FromString(TEXT("Accepts a quest and adds it to the quest log.")));
    RegisterEvent<UCompleteQuestEvent>("CompleteQuest", FText::FromString(TEXT("Marks a quest as completed.")));
    RegisterEvent<UFailQuestEvent>("FailQuest", FText::FromString(TEXT("Marks a quest as failed.")));
    RegisterEvent<UAddItemEvent>("AddItem", FText::FromString(TEXT("Adds an item to the player's inventory.")));
    RegisterEvent<UAddStatEvent>("AddStat", FText::FromString(TEXT("Increases the player's stats.")));
    RegisterEvent<UStartBattleEvent>("StartBattle", FText::FromString(TEXT("Starts a battle encounter.")));
    RegisterEvent<UCreateQuestObjectiveEvent>("CreateQuestObjective", FText::FromString(TEXT("Spawns a quest objective actor at a specific location.")));
    RegisterEvent<UChangeDialogueContextEvent>("ChangeDialogueContext", FText::FromString(TEXT("Changes the value of the dialogue context. Used in the OnEnterEvent of a dialogue.")));
}

