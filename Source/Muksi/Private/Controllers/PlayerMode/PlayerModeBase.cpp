// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerModeBase.h"


void UPlayerModeBase::EnterMode(AMuksiPlayerController* PlayerController)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Enter Mode"));
	PC = PlayerController;
}

void UPlayerModeBase::ExitMode()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Exit Mode"));
}

void UPlayerModeBase::HandleLeftClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleLeftClick"));
}

void UPlayerModeBase::HandleRightClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleRightClick"));
}


