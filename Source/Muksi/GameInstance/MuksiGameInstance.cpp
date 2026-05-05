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
    Dialogue->InitializeSubsystem();

    // 2. Trigger Table 기반 초기화
    Trigger->InitializeSubsystem();
}
