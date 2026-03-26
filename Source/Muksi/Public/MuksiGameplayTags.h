// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NativeGameplayTags.h"

namespace MuksiGameplayTag
{
	//Muksi Widget Stack
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_WidgetStack_Modal);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_WidgetStack_GameMenu);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_WidgetStack_GameHud);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_WidgetStack_Frontend);
	
	//Muksi Widgets
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Widget_MainMenu_PressAnyKeyScreen);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Widget_MainMenu_MainMenuScreen);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Widget_MainMenu_ConfirmScreen);
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Widget_MainMenu_OptionsScreen);
	
	//Muksi Battle Widgets
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Widget_Battle_MainScreen);
	
	
	//Muksi Options Image
	MUKSI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muksi_Image_TestImage);
}

