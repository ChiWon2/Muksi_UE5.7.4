// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleUserWidget/BattleUserWidgetBase.h"

void UBattleUserWidgetBase::AnimEventCalled()
{
	OnAnimationEvent.Broadcast();
}
