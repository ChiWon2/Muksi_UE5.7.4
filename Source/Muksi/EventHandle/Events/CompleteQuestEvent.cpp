// Fill out your copyright notice in the Description page of Project Settings.


#include "CompleteQuestEvent.h"

void UCompleteQuestEvent::Execute(const FEventParameter& Param)
{
	FName QuestID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[UCompleteQuestEvent] : UCompleteQuestEvent Executed!! QuestID : \"%s\" is Completed by QuestSystem"),
		*QuestID.ToString());
}
