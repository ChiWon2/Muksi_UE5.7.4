// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"

#include "Components/Button.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Enemy.h"




void UWidget_CharacterData::CloseActivatableWidget()
{
	//TODO Close Effect
	DeactivateWidget();
}

void UWidget_CharacterData::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CloseBackgroundButton)
	{
		CloseBackgroundButton->OnClicked.AddDynamic(this, &UWidget_CharacterData::CloseActivatableWidget);
	}
	SetWidgetVisible();
	
}

void UWidget_CharacterData::NativeOnDeactivated()
{
	/*UE_LOG(LogTemp, Log, TEXT("Widget_CharacterData Deactivated!!"));
	if (AMuksiPlayerController* PC = GetOwningMuksiPlayerController())
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		/*FInputModeGameAndUI InputMode;
		//FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		Debug::Print(TEXT("Deactivated CharacterData"));#1#
		
		/*CloseBackgroundButton->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayerDeckDataWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		EnemyDeckDataWidget->SetVisibility(ESlateVisibility::HitTestInvisible);#1#
	}*/
	Super::NativeOnDeactivated();
	
}

void UWidget_CharacterData::GetCharacterData(ABattleCharacterBase* CharacterData)
{
	PlayerData = nullptr;
	EnemyData = nullptr;
	
	UE_LOG(LogTemp, Warning, TEXT("GetCharacterData this: %s / %p"), *GetNameSafe(this), this);
	
	if (ABattleCharacter_Player* Player = Cast<ABattleCharacter_Player>(CharacterData))
	{
		PlayerData = Player;
	}else if (ABattleCharacter_Enemy* Enemy = Cast<ABattleCharacter_Enemy>(CharacterData))
	{
		EnemyData = Enemy;
	}
	
	SetWidgetVisible();
}

void UWidget_CharacterData::SetWidgetVisible()
{
	if (PlayerData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Data Print"));
		PlayerDataPanelWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerDataPanelWidget->InitializeFromPlayerMode();
		PlayerDataPanelWidget->ApplyCharacterData(PlayerData);
		EnemyDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
	}else if (EnemyData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Data Print"));
		PlayerDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
		EnemyDataPanelWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

