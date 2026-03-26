// Fill out your copyright notice in the Description page of Project Settings.


#include "AcceptQuestEvent.h"
//#include"../EventHandleSubsystem.h"
//#include"../../Quest/QuestSubsystem.h"
//#include"../../Dialogue/DialogueQuestGameInstance.h"

void UAcceptQuestEvent::Execute(const FEventParameter& Param)
{
	FName QuestID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[AcceptQuestEvent] : AcceptQuestEvent Executed!! QuestID : \"%s\" is Add into your QuestSystem"),
		*QuestID.ToString());
	//
	//UEventHandleSubsystem* EventSubsystem =GetTypedOuter<UEventHandleSubsystem>();
	//UQuestSubsystem* QuestSubsystem =EventSubsystem->GetGameInstance()->GetSubsystem<UQuestSubsystem>();

	//QuestSubsystem->AddNewQuest(QuestID);
}
