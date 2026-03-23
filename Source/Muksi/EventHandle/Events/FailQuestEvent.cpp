// Fill out your copyright notice in the Description page of Project Settings.


#include "FailQuestEvent.h"

void UFailQuestEvent::Execute(const FEventParameter& Param)
{
	FName QuestID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[UFailQuestEvent] : QuestID : \"%s\" is Failed by QuestSystem"),
		*QuestID.ToString());
}