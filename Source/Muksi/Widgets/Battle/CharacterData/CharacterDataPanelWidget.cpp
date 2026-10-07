// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget.h"

#include "CommonAnimatedSwitcher.h"
#include "Widget_CharacterDeckPanel.h"
#include "Components/Button.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Widgets/Battle/Passive/Widget_CharacterPassivePanel.h"
#include "Player/Widget_PlayerProfilePanel.h"
#include "Slate/SObjectWidget.h"


void UCharacterDataPanelWidget::ApplyCharacterData(ABattleCharacterBase* CharacterData)
{
	if (!CharacterData)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CharacterDataPanelWidget - PlayerData is null")
		);
		return;
	}

	PlayerProfilePanelWidget->SetBattleCharacter(CharacterData);
	PlayerProfilePanelWidget->SetData(CharacterData->GetCharacterData());

	UBattleSkillComponent* SkillComponent = CharacterData->GetBattleSkillComponent();

	if (SkillComponent)
	{
		CharacterDeckPanelWidget->SetDeckData(SkillComponent->GetSkillDataList());
	}

	CharacterPassivePanelWidget->SetPassiveData(CharacterData->GetCharacterPassives());
}

void UCharacterDataPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Profile)
	{
		Button_Profile->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget::OnProfileButtonClicked);
	}

	if (Button_Skill)
	{
		Button_Skill->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget::OnSkillButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.AddDynamic(this, &UCharacterDataPanelWidget::OnPassiveButtonClicked);
	}

	// 처음 열렸을 때 기본으로 프로필 패널 표시
	SwitchCharacterPanel(ECharacterDataPanel::Profile);
}

void UCharacterDataPanelWidget::NativeDestruct()
{
	if (Button_Profile)
	{
		Button_Profile->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget::OnProfileButtonClicked);
	}

	if (Button_Skill)
	{
		Button_Skill->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget::OnSkillButtonClicked);
	}
	
	if (Button_Passive)
	{
		Button_Passive->OnClicked.RemoveDynamic(this, &UCharacterDataPanelWidget::OnPassiveButtonClicked);
	}
	
	Super::NativeDestruct();
}




void UCharacterDataPanelWidget::OnProfileButtonClicked()
{
	SwitchCharacterPanel(ECharacterDataPanel::Profile);
}

void UCharacterDataPanelWidget::OnSkillButtonClicked()
{
	SwitchCharacterPanel(ECharacterDataPanel::Skill);
}

void UCharacterDataPanelWidget::OnPassiveButtonClicked()
{
	SwitchCharacterPanel(ECharacterDataPanel::Passive);
}

void UCharacterDataPanelWidget::SwitchCharacterPanel(ECharacterDataPanel PanelType)
{
	if (!WidgetSwitcher)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CharacterDataPanelWidget_Player - WidgetSwitcher is null")
		);
		return;
	}

	CurrentPanel = PanelType;

	switch (CurrentPanel)
	{
	case ECharacterDataPanel::Profile:
		WidgetSwitcher->SetActiveWidgetIndex(0);
		break;

	case ECharacterDataPanel::Skill:
		WidgetSwitcher->SetActiveWidgetIndex(1);
		break;

	case ECharacterDataPanel::Passive:
		WidgetSwitcher->SetActiveWidgetIndex(2);
		break;

	default:
		return;
	}

	UpdateButtonState();
}

void UCharacterDataPanelWidget::UpdateButtonState()
{
	SetButtonSelected(Button_Profile,CurrentPanel == ECharacterDataPanel::Profile);
	SetButtonSelected(Button_Skill,CurrentPanel == ECharacterDataPanel::Skill);
	SetButtonSelected(Button_Passive,CurrentPanel == ECharacterDataPanel::Passive);
}

void UCharacterDataPanelWidget::SetButtonSelected(UButton* Button, bool bSelected)
{
	if (!Button)
	{
		return;
	}

	const FSlateBrush& TargetBrush =
		bSelected
			? SelectedButtonBrush
			: UnselectedButtonBrush;

	FButtonStyle NewStyle = Button->GetStyle();

	NewStyle.SetNormal(TargetBrush);
	NewStyle.SetHovered(TargetBrush);
	NewStyle.SetPressed(TargetBrush);

	Button->SetStyle(NewStyle);

	// 원본 이미지 색상을 그대로 사용
	Button->SetBackgroundColor(FLinearColor::White);
}
