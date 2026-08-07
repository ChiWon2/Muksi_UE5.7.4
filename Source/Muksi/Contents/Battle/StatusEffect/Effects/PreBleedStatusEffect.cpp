// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/PreBleedStatusEffect.h"

void UPreBleedStatusEffect::OnRoundEnd()
{
	//TODO 국 종료시 해당 스택만큼 BleedStatusEffect로 변환하기
	
	SetStack(0);
}
