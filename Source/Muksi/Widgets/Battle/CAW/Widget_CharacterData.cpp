// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"

#include "Controllers/MuksiPlayerController.h"
#include "Components/Button.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Enemy.h"

#include "MuksiDebugHelper.h"


void UWidget_CharacterData::CloseActivatabelWidget()
{
	//TODO Close Effect
	DeactivateWidget();
}

void UWidget_CharacterData::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CloseBackgroundButton)
	{
		CloseBackgroundButton->OnClicked.AddDynamic(this, &UWidget_CharacterData::CloseActivatabelWidget);
	}
	
	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	if (ABattleCharacter_Player* Player = Cast<ABattleCharacter_Player>(PC->ClickedActor))
	{
		PlayerCharacterData = Player;
		
		Debug::Print(TEXT("Player Data Print"));
		PlayerDataPanelWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerDataPanelWidget->InitializeFromPlayerMode();
		EnemyDeckDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
	}else if (ABattleCharacter_Enemy* Enemy = Cast<ABattleCharacter_Enemy>(PC->ClickedActor))
	{
		EnemyCharacterData = Enemy;
		
		Debug::Print(TEXT("Enemy Data Print"));
		
		PlayerDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
		EnemyDeckDataPanelWidget->SetVisibility(ESlateVisibility::Visible);
	}
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
