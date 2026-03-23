// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeDialogueContextEvent.h"
//#include"../EventHandleSubsystem.h"
//#include"../../Dialogue/DialogueSubsystem.h"

void UChangeDialogueContextEvent::Execute(const FEventParameter& Param)
{
	FName Key = Param.NameValue;
	FText Value = Param.TextValue;

	//UEventHandleSubsystem* EventSubsystem = GetTypedOuter<UEventHandleSubsystem>();
	//UDialogueSubsystem* DSS = EventSubsystem->GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

	//DSS->AddDialogueContext(Key, Value);
}
