// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/Player/Widget_PlayerProfilePanel.h"

#include "Components/Image.h"
#include "Components/Slider.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Widgets/Battle/CharacterData/VerticalCommonTextBlock.h"

void UWidget_PlayerProfilePanel::SetData(UMuksiCharacterDataAsset* DataAsset)
{
	//이미지
	PlayerImage->SetBrushFromTexture(DataAsset->CharacterIllustration);
	//캐릭터 이름
	PlayerName->SetVerticalText(DataAsset->CharacterName);
	//캐릭터 소속
	PlayerIntroduction->SetVerticalText(DataAsset->FactionDescription);
	//캐릭터 체력
	MaxHP = DataAsset->MaxHP;
	SetHP(DataAsset->MaxHP);
	if (!PlayerCharacter){UE_LOG(LogTemp, Error, TEXT("BattleCharacter is null (Widget_PlayerProfilePanel.cpp)"));return;}
	PlayerCharacter->BattleStatComponent->OnHPChanged.AddDynamic(this, &UWidget_PlayerProfilePanel::HPChanged);
}

void UWidget_PlayerProfilePanel::SetHP(int32 CurrentHP)
{
	if (!IsValid(PlayerHPSlider))
	{
		return;
	}

	if (MaxHP <= 0)
	{
		PlayerHPSlider->SetValue(0.0f);
		return;
	}

	const float HPRatio = static_cast<float>(CurrentHP)
		/ static_cast<float>(MaxHP);

	PlayerHPSlider->SetValue(FMath::Clamp(HPRatio, 0.0f, 1.0f));
}

void UWidget_PlayerProfilePanel::HPChanged(float PreHP, float AftHP)
{
	if (AftHP - PreHP <= 0.0f)
	{
		HPDown(PreHP, AftHP);
	}else
	{
		HPUp(PreHP, AftHP);
	}
}

//대미지 받는 효과/ 힐하는 효과 다르게 표시 할 수 있기에 나눠놈
void UWidget_PlayerProfilePanel::HPUp(float PreHP, float AftHP)
{
	const float HPRatio = static_cast<float>(AftHP)
		/ static_cast<float>(MaxHP);
	PlayerHPSlider->SetValue(FMath::Clamp(HPRatio, 0.0f, 1.0f));
}

void UWidget_PlayerProfilePanel::HPDown(float PreHP, float AftHP)
{
	const float HPRatio = static_cast<float>(AftHP)
		/ static_cast<float>(MaxHP);
	PlayerHPSlider->SetValue(FMath::Clamp(HPRatio, 0.0f, 1.0f));
}


