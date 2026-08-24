// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Enemy.h"

#include "CommonAnimatedSwitcher.h"
#include "Widget_CharacterDeckPanel.h"
#include "Components/Button.h"
#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Widgets/Battle/Passive/Widget_CharacterPassivePanel.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Player/Widget_PlayerProfilePanel.h"


void UCharacterDataPanelWidget_Enemy::InitializeFromPlayerMode()
{
	AMuksiPlayerController* MuksiPC = Cast<AMuksiPlayerController>(GetOwningPlayer());
	if (!MuksiPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Enemy - MuksiPC is null"));
		return;
	}

	UPlayerMode_Battle* PlayerMode = MuksiPC->GetPlayerMode();
	if (!PlayerMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Enemy - PlayerMode is null"));
		return;
	}

	UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
	ABattleManager* BattleManager = ManagerSubsystem
		? ManagerSubsystem->GetManager<ABattleManager>()
		: nullptr;
	UBattleRuntimeContext* BattleRuntimeContext = BattleManager
		? BattleManager->GetBattleRuntimeContext()
		: nullptr;

	ApplyCharacterData(BattleRuntimeContext ? BattleRuntimeContext->GetEnemyCharacter() : nullptr);
}

void UCharacterDataPanelWidget_Enemy::ApplyCharacterData(ABattleCharacter_Enemy* PlayerData)
{
	if (!PlayerData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDataPanelWidget_Player - InCharacterData is null"));
		return;
	}
	EnemyProfilePanelWidget->SetBattleCharacter(PlayerData);
	EnemyProfilePanelWidget->SetData(PlayerData->GetCharacterData());
	UBattleCardComponent* CardComponent = PlayerData->GetBattleCardComponent();
	CharacterDeckPanelWidget->SetDeckData(CardComponent->GetFullDeck());
	CharacterPassivePanelWidget->SetPassiveData(PlayerData->GetCharacterPassives());
}

void UCharacterDataPanelWidget_Enemy::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Profile)
	{
		Button_Profile->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Enemy::OnProfileButtonClicked);
	}

	if (Button_Deck)
	{
		Button_Deck->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Enemy::OnDeckButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget_Enemy::OnPassiveButtonClicked);
	}

	// 처음 열렸을 때 기본으로 프로필 패널 표시
	SwitchEnemyPanel(0);
}

void UCharacterDataPanelWidget_Enemy::NativeDestruct()
{
	if (Button_Profile)
	{
		Button_Profile->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Enemy::OnProfileButtonClicked);
	}

	if (Button_Deck)
	{
		Button_Deck->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Enemy::OnDeckButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget_Enemy::OnPassiveButtonClicked);
	}
	Super::NativeDestruct();
}

void UCharacterDataPanelWidget_Enemy::OnProfileButtonClicked()
{
	SwitchEnemyPanel(0);
}

void UCharacterDataPanelWidget_Enemy::OnDeckButtonClicked()
{
	SwitchEnemyPanel(1);
}

void UCharacterDataPanelWidget_Enemy::OnPassiveButtonClicked()
{
	SwitchEnemyPanel(2);
}

void UCharacterDataPanelWidget_Enemy::SwitchEnemyPanel(int32 PanelIndex)
{
	if (!WidgetSwitcher)
	{
		UE_LOG(LogTemp, Warning, TEXT("WidgetSwitcher_PlayerInfo is null"));
		return;
	}

	WidgetSwitcher->SetActiveWidgetIndex(PanelIndex);
	
}
