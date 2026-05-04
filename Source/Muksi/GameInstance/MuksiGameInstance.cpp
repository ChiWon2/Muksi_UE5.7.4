// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiGameInstance.h"
#include"../Dialogue/DialogueSubsystem.h"
#include"../Dialogue/DialogueTriggerSubsystem.h"
#include"../Dialogue/DeveloperSettings/DialogueDeveloperSettings.h"
#include "../Dialogue/Widgets/DialogueWidget.h"

void UMuksiGameInstance::Init()
{
    Super::Init();

    InitialzieDialogueSystem();
}

void UMuksiGameInstance::InitialzieDialogueSystem()
{
    const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();

    if (!Settings)
    {
        return;
    }
    UDialogueSubsystem* Dialogue = GetSubsystem<UDialogueSubsystem>();
    UDialogueTriggerSubsystem* Trigger = GetSubsystem<UDialogueTriggerSubsystem>();

    // 1. Dialogue Table 세팅
    Dialogue->SetDialogueTable(Settings->DialogueTables[0].Table.LoadSynchronous());

    // 2. Trigger Table 기반 초기화
    Trigger->FillUpTriggerIDs(Dialogue->GetDialogueTable());

    // 3. Widget Class 전달
    Dialogue->SetDialogueWidget(Settings->DialogueWidgetClass.LoadSynchronous());
}
