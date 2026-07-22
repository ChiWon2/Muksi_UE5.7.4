// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Player.h"

#include "Components/Button.h"
#include "CommonAnimatedSwitcher.h"
#include "Controllers/MuksiPlayerController.h"
#include "Controllers/PlayerMode/PlayerMode_Battle.h"
#include "Muksi/Widgets/Battle/CharacterData/Widget_CharacterDeckPanel.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Widgets/Battle/Passive/Widget_CharacterPassivePanel.h"

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

	ApplyCharacterData(Cast<ABattleCharacter_Player>(PlayerMode->GetPlayerCharacterData()));
}

void UCharacterDataPanelWidget_Player::ApplyCharacterData(ABattleCharacter_Player* PlayerData)
{
	if (!PlayerData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Player - InCharacterData is null"));
		return;
	}
	CharacterDeckPanelWidget->SetDeckData(PlayerData->GetAllBattleDeck());
	CharacterPassivePanelWidget->SetPassiveData(PlayerData->GetCharacterPassives());
	
}

void UCharacterDataPanelWidget_Player::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Profile)
	{
		Button_Profile->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Player::OnProfileButtonClicked);
	}

	if (Button_Deck)
	{
		Button_Deck->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Player::OnDeckButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Player::OnPassiveButtonClicked);
	}

	// 처음 열렸을 때 기본으로 프로필 패널 표시
	SwitchPlayerPanel(0);
}

void UCharacterDataPanelWidget_Player::NativeDestruct()
{
	if (Button_Profile)
	{
		Button_Profile->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Player::OnProfileButtonClicked);
	}

	if (Button_Deck)
	{
		Button_Deck->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Player::OnDeckButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Player::OnPassiveButtonClicked);
	}
	
	Super::NativeDestruct();
}

void UCharacterDataPanelWidget_Player::OnProfileButtonClicked()
{
	SwitchPlayerPanel(0);
}

void UCharacterDataPanelWidget_Player::OnDeckButtonClicked()
{
	SwitchPlayerPanel(1);
}

void UCharacterDataPanelWidget_Player::OnPassiveButtonClicked()
{
	SwitchPlayerPanel(2);
}

void UCharacterDataPanelWidget_Player::SwitchPlayerPanel(int32 PanelIndex)
{
	if (!WidgetSwitcher)
	{
		UE_LOG(LogTemp, Warning, TEXT("WidgetSwitcher_PlayerInfo is null"));
		return;
	}

	WidgetSwitcher->SetActiveWidgetIndex(PanelIndex);

	UE_LOG(LogTemp, Log, TEXT("SwitchPlayerPanel: %d"), PanelIndex);
}
