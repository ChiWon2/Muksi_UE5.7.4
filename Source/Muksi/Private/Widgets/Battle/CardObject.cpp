// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/CardObject.h"

void UCardObject::InitCard(const FName& InCardID, const FText& InCardName)
{
	CardID = InCardID;
	CardName = InCardName;
}
