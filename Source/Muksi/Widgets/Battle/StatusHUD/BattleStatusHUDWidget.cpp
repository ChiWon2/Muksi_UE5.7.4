// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatusHUD/BattleStatusHUDWidget.h"

#include "Muksi/Widgets/Battle/Status/CharacterStatusWidget.h"

void UBattleStatusHUDWidget::SetData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	if (CharacterStatusWidget_Player)
	{
		CharacterStatusWidget_Player->SetData(Player);
	}

	if (CharacterStatusWidget_Enemy)
	{
		CharacterStatusWidget_Enemy->SetData(Enemy);
	}
}

void UBattleStatusHUDWidget::SetPlayerCostPreview(int32 Cost)
{
	if (CharacterStatusWidget_Player)
	{
		CharacterStatusWidget_Player->SetCostPreview(Cost);
	}
}

void UBattleStatusHUDWidget::ClearPlayerCostPreview()
{
	if (CharacterStatusWidget_Player)
	{
		CharacterStatusWidget_Player->ClearCostPreview();
	}
}


