// Fill out your copyright notice in the Description page of Project Settings.


#include "AddStatEvent.h"

void UAddStatEvent::Execute(const FEventParameter& Param)
{
	int StatQuantity = Param.IntValue;
	FName StatID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[AddStatEvent] : AddStatEvent Executed!! \"%i\" numbers of \"%s\" is Add into your Stats"),
		StatQuantity,
		*StatID.ToString());
}
