// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"

#include "Components/Button.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Player.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget_Enemy.h"




void UWidget_CharacterData::CloseActivatableWidget()
{
	//TODO Close Effect
	DeactivateWidget();
	//카메라 원래 탑뷰 위치로 옮기기
	ReturnCameraPosition();
}

void UWidget_CharacterData::NativeConstruct()
{
	const double StartTime = FPlatformTime::Seconds();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CharacterData Test] NativeConstruct Start")
	);
	
	Super::NativeConstruct();
	
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CharacterData Test] NativeConstruct Super End : %.2f ms"),
		(FPlatformTime::Seconds() - StartTime) * 1000.0
	);
	
	if (CloseBackgroundButton)
	{
		CloseBackgroundButton->OnClicked.RemoveDynamic(this, &UWidget_CharacterData::CloseActivatableWidget);
		CloseBackgroundButton->OnClicked.AddDynamic(this, &UWidget_CharacterData::CloseActivatableWidget);
	}
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CharacterData Test] NativeConstruct Total : %.2f ms"),
		(FPlatformTime::Seconds() - StartTime) * 1000.0
	);
}

void UWidget_CharacterData::NativeOnActivated()
{
	const double StartTime = FPlatformTime::Seconds();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CharacterData Test] NativeOnActivated Start")
	);
	Super::NativeOnActivated();
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CharacterData Test] NativeOnActivated End : %.2f ms"),
		(FPlatformTime::Seconds() - StartTime) * 1000.0
	);
}

void UWidget_CharacterData::NativeOnDeactivated()
{
	PlayerData = nullptr;
	EnemyData = nullptr;
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
		PlayerDataPanelWidget->ApplyCharacterData(PlayerData);
		EnemyDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
	}else if (EnemyData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Data Print"));
		EnemyDataPanelWidget->SetVisibility(ESlateVisibility::Visible);
		EnemyDataPanelWidget->ApplyCharacterData(EnemyData);
		PlayerDataPanelWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_CharacterData::ReturnCameraPosition()
{
	if (UWorld* World = GetWorld())
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem =
			World->GetSubsystem<UMuksiWorldManagerSubsystem>())
		{
			if (ABattleCameraManager* CameraManager =
				ManagerSubsystem->GetManager<ABattleCameraManager>())
			{
				CameraManager->ReturnToOverview();
			}
		}
	}
}

