// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Passive/PassiveActivePopupWidget.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Widgets/Battle/Passive/PassiveActivePopup.h"

void UPassiveActivePopupWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PopupQueueTimerHandle);
	}

	PendingPopups.Empty();
	Super::NativeDestruct();
}

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
	EnqueuePopup(CharacterImage, PassiveName, true);
}

void UPassiveActivePopupWidget::SetPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName)
{
	EnqueuePopup(CharacterImage, PassiveName, false);
}

void UPassiveActivePopupWidget::EnqueuePopup(UTexture2D* CharacterImage, FText PassiveName, bool bIsPlayer)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FPendingPassivePopup Popup;
	Popup.Image = CharacterImage;
	Popup.PassiveName = PassiveName;
	Popup.bIsPlayer = bIsPlayer;

	PendingPopups.Add(Popup);

	// 이미 처리 중이면 큐에 추가만 한다.
	if (World->GetTimerManager().IsTimerActive(PopupQueueTimerHandle))
	{
		return;
	}

	// 다음 팝업부터는 0.2초 간격으로 처리한다.
	World->GetTimerManager().SetTimer(
		PopupQueueTimerHandle,
		this,
		&UPassiveActivePopupWidget::ProcessNextPopup,
		0.2f,
		true
	);

	// 첫 팝업은 즉시 표시한다.
	ProcessNextPopup();
}

void UPassiveActivePopupWidget::ProcessNextPopup()
{
	if (PendingPopups.IsEmpty())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PopupQueueTimerHandle);
		}
		return;
	}

	const FPendingPassivePopup Popup = PendingPopups[0];
	PendingPopups.RemoveAt(0);

	if (Popup.bIsPlayer)
	{
		ShowPassiveInfo_Player(Popup.Image.Get(), Popup.PassiveName);
	}
	else
	{
		ShowPassiveInfo_Enemy(Popup.Image.Get(), Popup.PassiveName);
	}
}

void UPassiveActivePopupWidget::ShowPassiveInfo_Player(UTexture2D* CharacterImage, FText PassiveName)
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
	NewPopup->StartRemoveTimer(3.0f);

	PopupSlot->SetHorizontalAlignment(HAlign_Fill);
	PopupSlot->SetVerticalAlignment(VAlign_Top);
	PopupSlot->SetPadding(PopupPadding);
}

void UPassiveActivePopupWidget::ShowPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName)
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
	NewPopup->StartRemoveTimer(3.0f);

	PopupSlot->SetHorizontalAlignment(HAlign_Fill);
	PopupSlot->SetVerticalAlignment(VAlign_Top);
	PopupSlot->SetPadding(PopupPadding);
}
