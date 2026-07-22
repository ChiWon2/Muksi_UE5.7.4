// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Passive/Widget_CharacterPassivePanel.h"

#include "PassiveInfoPanel.h"
#include "Components/ScrollBox.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

void UWidget_CharacterPassivePanel::SetPassiveData(TArray<UCharacterPassive*> InPassiveArray)
{
	PassiveInfoPanelArray.Empty();

	for (UCharacterPassive* PassiveData : InPassiveArray)
	{
		
		UPassiveInfoPanel* PassiveInfoPanel = CreateWidget<UPassiveInfoPanel>(GetOwningPlayer(), PassiveInfoPanelClass);
		if (!PassiveInfoPanel)
		{
			UE_LOG(LogTemp, Error, TEXT("PassiveData is null (Widget_CharacterPassivePanel.cpp)"));
			return;
		}
		PassiveInfoPanel->SetPassiveData(PassiveData);
		PassiveInfoPanelArray.Add(PassiveInfoPanel);
	}

	RefreshPassivePanel();
}

void UWidget_CharacterPassivePanel::RefreshPassivePanel()
{
	ClearPassivePanel();

	if (!PassiveScrollBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("PassiveScroll Box is null (Widget_CharacterPassivePanel.cpp)"));
		return;
	}

	for (UPassiveInfoPanel* PassiveInfoPanel : PassiveInfoPanelArray)
	{
		if (!PassiveInfoPanel)continue;
		PassiveScrollBox->AddChild(PassiveInfoPanel);
	}
}

void UWidget_CharacterPassivePanel::ClearPassivePanel()
{
	if (PassiveScrollBox)
	{
		PassiveScrollBox->ClearChildren();
	}
}
