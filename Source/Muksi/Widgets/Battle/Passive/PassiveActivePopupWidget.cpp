// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopup.h"

void UPassiveActivePopupWidget::SetData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	//Player
	UCharacterPassiveComponent* PlayerPassiveComponent = Player->GetPassiveComponent();
	for (UCharacterPassive* CharacterPassive : PlayerPassiveComponent->GetCharacterPassives())
	{
		CharacterPassive->OnPassiveActive.AddDynamic(this, &UPassiveActivePopupWidget::SetPassiveInfo_Player);
	}
	
	//Enemy
	UCharacterPassiveComponent* EnemyPassiveComponent = Enemy->GetPassiveComponent();
	for (UCharacterPassive* CharacterPassive : EnemyPassiveComponent->GetCharacterPassives())
	{
		CharacterPassive->OnPassiveActive.AddDynamic(this, &UPassiveActivePopupWidget::SetPassiveInfo_Enemy);
	}
}

void UPassiveActivePopupWidget::SetPassiveInfo_Player(UTexture2D* CharacterImage, FText PassiveName)
{
	if (!PassiveActivePopupClass_Player)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PassiveActivePopupClass_Player is not set")
		);
		return;
	}

	if (!IsValid(VerticalBox_Player))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("VerticalBox_Player is invalid")
		);
		return;
	}

	APlayerController* OwningPlayer = GetOwningPlayer();

	if (!IsValid(OwningPlayer))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GetOwningPlayer() is invalid")
		);
		return;
	}

	UPassiveActivePopup* NewPopup =
		CreateWidget<UPassiveActivePopup>(
			OwningPlayer,
			PassiveActivePopupClass_Player
		);

	if (!IsValid(NewPopup))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to create PassiveActivePopup")
		);
		return;
	}

	UVerticalBoxSlot* PopupSlot =
		VerticalBox_Player->AddChildToVerticalBox(NewPopup);

	if (!IsValid(PopupSlot))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to add NewPopup to VerticalBox_Player")
		);
		return;
	}

	NewPopup->SetData(CharacterImage, PassiveName);

	PopupSlot->SetHorizontalAlignment(HAlign_Fill);
	PopupSlot->SetVerticalAlignment(VAlign_Top);
	PopupSlot->SetPadding(
		FMargin(0.0f, 0.0f, 0.0f, 8.0f)
	);
}

void UPassiveActivePopupWidget::SetPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName)
{
	if (!PassiveActivePopupClass_Enemy)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PassiveActivePopupClass_Player is not set")
		);
		return;
	}

	if (!IsValid(VerticalBox_Enemy))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("VerticalBox_Player is invalid")
		);
		return;
	}

	APlayerController* OwningPlayer = GetOwningPlayer();

	if (!IsValid(OwningPlayer))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("GetOwningPlayer() is invalid")
		);
		return;
	}

	UPassiveActivePopup* NewPopup =
		CreateWidget<UPassiveActivePopup>(
			OwningPlayer,
			PassiveActivePopupClass_Enemy
		);

	if (!IsValid(NewPopup))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to create PassiveActivePopup")
		);
		return;
	}

	UVerticalBoxSlot* PopupSlot =
		VerticalBox_Enemy->AddChildToVerticalBox(NewPopup);

	if (!IsValid(PopupSlot))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to add NewPopup to VerticalBox_Player")
		);
		return;
	}

	NewPopup->SetData(CharacterImage, PassiveName);

	PopupSlot->SetHorizontalAlignment(HAlign_Fill);
	PopupSlot->SetVerticalAlignment(VAlign_Top);
	PopupSlot->SetPadding(
		FMargin(0.0f, 0.0f, 0.0f, 8.0f)
	);
}
