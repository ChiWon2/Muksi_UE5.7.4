// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/Widget_BattleMainScreen.h"

#include "Components/Button.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "MuksiGameplayTags.h"
#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/HandWidget.h"
#include "Muksi/Widgets/Components/InkLineWidget.h"

#include "MuksiDebugHelper.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"



void UWidget_BattleMainScreen::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	GetOwningMuksiPlayerController()->TestWidgetScreen = this;
	
	//Debug::Print(TEXT("TestNativeConstruct"));
	/*if (AMuksiPlayerController* PC = GetOwningMuksiPlayerController())
	{
		Debug::Print(TEXT("TestNativeConstruct"));
		//FInputModeUIOnly InputMode;
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}*/
}

FReply UWidget_BattleMainScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	Debug::Print(TEXT("ActivatableBase MouseDown"));
	return FReply::Unhandled();
}

void UWidget_BattleMainScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	//UE_LOG(LogTemp, Warning, TEXT("Frontend Reactivated"));
	RequestRefreshFocus();
	
	if (AMuksiPlayerController* PC = GetOwningMuksiPlayerController())
	{
		Debug::Print(TEXT("TestNativeOnActivated"));
		//FInputModeUIOnly InputMode;
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
}


