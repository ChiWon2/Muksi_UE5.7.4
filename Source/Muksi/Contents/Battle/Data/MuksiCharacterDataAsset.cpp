// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

#include "Muksi/Contents/Battle/Data/BattleCharacterInitData.h"

FBattleCharacterInitData UMuksiCharacterDataAsset::MakeBattleInitData()
{
	FBattleCharacterInitData InitData;

	InitData.CharacterClass = BattleCharacterClass;

	InitData.CharacterInfo.CharacterID = CharacterID;
	InitData.CharacterInfo.CharacterName = CharacterName;
	InitData.CharacterInfo.CharacterIllustration = CharacterIllustration;
	InitData.CharacterInfo.CharacterHUD = CharacterHUD;
	InitData.CharacterInfo.FactionName = FactionName;
	InitData.CharacterInfo.FactionDescription = FactionDescription;

	InitData.Stats.HP = MaxHP;
	InitData.Stats.Attack = AttackValue;
	InitData.Stats.Defense = DefenseValue;
	InitData.Stats.Speed = CharacterSpeed;

	// 기존 반환 순서와 동일하게 X=최소, Y=최대.
	InitData.Stats.SelectSpeed = FVector2D(
		CardSelectTimeMin,
		CardSelectTimeMax);

	InitData.CurrentHP = MaxHP;

	InitData.Skills = CharacterDeck;
	InitData.MaxSkillCost = MaxSkillCost;
	InitData.PassiveClasses = CharacterPassiveClass;
	InitData.TimeoutPenalties = TimeoutPenalties;

	return InitData;
}
