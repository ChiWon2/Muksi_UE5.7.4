// Fill out your copyright notice in the Description page of Project Settings.


#include "AddItemEvent.h"

void UAddItemEvent::Execute(const FEventParameter& Param)
{
	int ItemQuantity = Param.IntValue;
	FName ItemID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[AddItemEvent] : ItemEvent Executed!! \"%i\" numbers of \"%s\" is Add into your inventory"),
		ItemQuantity,
		*ItemID.ToString());
}