// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatusHUD/BattleStatusHUDWidget.h"

#include "Muksi/Widgets/Battle/Status/CharacterStatusWidget.h"

void UBattleStatusHUDWidget::SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
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
