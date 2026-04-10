// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CAW/Widget_Test.h"

#include "Controllers/MuksiPlayerController.h"

#include "MuksiDebugHelper.h"

void UWidget_Test::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AMuksiPlayerController* PC = GetOwningMuksiPlayerController())
	{
		Debug::Print(TEXT("Widget_Test NativeConstruct Input Mode "));
		FInputModeGameAndUI InputMode;
		//FInputModeGameOnly InputMode;
		//FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}
