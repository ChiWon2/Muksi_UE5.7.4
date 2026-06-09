// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerMode_Test.h"
#include "Controllers/MuksiPlayerController.h"

void UPlayerMode_Test::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
}

void UPlayerMode_Test::ExitMode()
{

}
