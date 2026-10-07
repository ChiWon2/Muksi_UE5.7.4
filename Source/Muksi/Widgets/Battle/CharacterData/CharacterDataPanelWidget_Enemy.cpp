// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Enemy.h"






/*

void UCharacterDataPanelWidget_Enemy::ApplyCharacterData(ABattleCharacter_Enemy* PlayerData)
{
	if (!PlayerData)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CharacterDataPanelWidget_Enemy - PlayerData is null")
		);
		return;
	}

	EnemyProfilePanelWidget->SetBattleCharacter(PlayerData);
	EnemyProfilePanelWidget->SetData(PlayerData->GetCharacterData());

	UBattleSkillComponent* SkillComponent =
		PlayerData->GetBattleSkillComponent();

	if (SkillComponent)
	{
		CharacterDeckPanelWidget->SetDeckData(SkillComponent->GetSkillDataList());
	}

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
	
}*/
