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
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_Battle_CharacterData, "Muksi.Widget.Battle.CharacterData");
	
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_Battle_Test, "Muksi.Widget.Battle.Test");
	
	//Muksi TravelTime Widgets
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_TravelTime_ClockHUD, "Muksi.Widget.TravelTime.ClockHUD");

	//Muksi World Widgets
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_World_MainScreen, "Muksi.Widget.World.MainScreen");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_World_Town, "Muksi.Widget.World.Town");
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_World_InventoryEquipment, "Muksi.Widget.World.InventoryEquipment");


	//Muksi Dialogue Widgets
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Widget_Dialogue_Main, "Muksi.Widget.Dialogue.Main");
	
	//Muksi Options Image
	UE_DEFINE_GAMEPLAY_TAG(Muksi_Image_TestImage, "Muksi.Image.TestImage")
}