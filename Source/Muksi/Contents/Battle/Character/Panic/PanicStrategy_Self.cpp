// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Panic/PanicStrategy_Self.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

FPanicStrategyResult UPanicStrategy_Self::SelectTarget_Implementation(const FPanicStrategyContext& Context)
{
	FPanicStrategyResult Result;

	if (!IsValid(Context.SourceCharacter))
	{
		return Result;
	}

	const FHexOffsetCoord SourceCoord =
		Context.SourceCharacter->GetCharacterCoord();

	if (!SourceCoord.IsValid())
	{
		return Result;
	}

	Result.TargetCoord = SourceCoord;
	Result.Direction = INDEX_NONE;

	return Result;
}
