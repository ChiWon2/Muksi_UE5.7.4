// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerModeBase.h"


void UPlayerModeBase::EnterMode()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Enter Mode"));
}

void UPlayerModeBase::ExitMode()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Exit Mode"));
}
