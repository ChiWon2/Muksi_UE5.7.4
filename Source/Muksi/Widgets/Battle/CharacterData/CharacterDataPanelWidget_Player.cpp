// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Player.h"

#include "Controllers/MuksiPlayerController.h"
#include "Controllers/PlayerMode/PlayerMode_Battle.h"
#include "Muksi/Widgets/Battle/CharacterData/Widget_CharacterDeckPanel.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"

void UCharacterDataPanelWidget_Player::InitializeFromPlayerMode()
{
	AMuksiPlayerController* MuksiPC = Cast<AMuksiPlayerController>(GetOwningPlayer());
	if (!MuksiPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Player - MuksiPC is null"));
		return;
	}

	UPlayerMode_Battle* PlayerMode = MuksiPC->GetPlayerMode();
	if (!PlayerMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Player - PlayerMode is null"));
		return;
	}

	ApplyCharacterData(PlayerMode->GetPlayerCharacterData());
}

void UCharacterDataPanelWidget_Player::ApplyCharacterData(UCharacterDataBase* InCharacterData)
{
	if (!InCharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Player - InCharacterData is null"));
		return;
	}

	if (CharacterDeckPanelWidget)
	{
		CharacterDeckPanelWidget->SetDeckData(
			InCharacterData->GetCardDataTable(),
			InCharacterData->GetDeckCardRowNames()
		);
	}
}
