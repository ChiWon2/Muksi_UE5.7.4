// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiGameplayTags.h"

namespace MuksiGameplayTag
{
	//Muksi Widget stack
	UE_DEFINE_GAMEPLAY_TAG(Muksi_WidgetStack_Modal, "Muksi.WidgetStack.Modal");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_WidgetStack_GameMenu, "Muksi.WidgetStack.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_WidgetStack_GameHud, "Muksi.WidgetStack.GameHud");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_WidgetStack_Frontend, "Muksi.WidgetStack.Frontend");
	
	//Muksi MainMenu Widgets
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_MainMenu_PressAnyKeyScreen, "Muksi.Widget.MainMenu.PressAnyKeyScreen");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_MainMenu_MainMenuScreen, "Muksi.Widget.MainMenu.MainMenuScreen");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_MainMenu_ConfirmScreen, "Muksi.Widget.MainMenu.ConfirmScreen");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_MainMenu_OptionsScreen, "Muksi.Widget.MainMenu.OptionsScreen");
	
	//Muksi Battle Widgets
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_BattleMainScreen, "Muksi.Widget.Battle.MainScreen");
	
	//Muksi Options Image
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Image_TestImage, "Muksi.Image.TestImage")
}