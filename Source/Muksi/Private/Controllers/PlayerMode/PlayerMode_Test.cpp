// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerMode_Test.h"
#include "../Public/Subsystems/MuksiUISubsystem.h"
#include"../Public/MuksiGameplayTags.h"
#include"../Quest/DeveloperSettings/QuestDeveloperSettings.h"
#include"../Quest/Widgets/QuestLogWidget.h"


void UPlayerMode_Test::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);
}

void UPlayerMode_Test::ExitMode()
{
	Super::ExitMode();
}

void UPlayerMode_Test::HandleQPressedKey(const FInputActionValue& Value)
{
    const UQuestDeveloperSettings* QuestDevSetting = GetDefault<UQuestDeveloperSettings>();
    TSoftClassPtr<UWidget_ActivatableBase> WidgetClass(QuestDevSetting->QuestLogWidgetClass);

    UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
        nullptr,
        MuksiGameplayTag::Muksi_WidgetStack_Modal,
        WidgetClass,
        true,
        nullptr,
        [this](UWidget_ActivatableBase* CreatedWidget)
        {
        }
    );
}
