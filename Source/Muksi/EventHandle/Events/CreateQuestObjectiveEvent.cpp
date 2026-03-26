// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateQuestObjectiveEvent.h"
//#include"../../Quest/Objectives/QuestObjectiveCreator.h"
//#include"../EventHandleSubsystem.h"
//#include"../../Dialogue/DialogueQuestGameInstance.h"

void UCreateQuestObjectiveEvent::Execute(const FEventParameter& Param)
{
    FName ObjectiveID = Param.NameValue;
    FVector CreatePos = Param.VectorValue;

    UE_LOG(LogTemp,
        Warning,
        TEXT("[CreateQuestObjectiveEvent] : CreateQuestObjectiveEvent Executed!! ObjectiveID : \"%s\" is Created into your World"),
        *ObjectiveID.ToString());

    //UEventHandleSubsystem* EventSubsystem = GetTypedOuter<UEventHandleSubsystem>();
    //
    //if (!EventSubsystem)
    //    return;

    //UDialogueQuestGameInstance* GI =Cast<UDialogueQuestGameInstance>(EventSubsystem->GetGameInstance());
    //if (!GI)
    //    return;

    //AQuestObjectiveCreator* ObjectiveCreator = GI->GetObjectiveCreator();
    //
    //if (!ObjectiveCreator)
    //    return;

    //FTransform SpawnTransform(CreatePos);

    //ObjectiveCreator->CreateObjectiveTarget(ObjectiveID, SpawnTransform);
}
