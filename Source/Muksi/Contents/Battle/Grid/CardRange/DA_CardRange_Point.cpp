// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/DA_CardRange_Point.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> UDA_CardRange_Point::GetRangeCoords(const ABattleGridManager* GridManager, FIntPoint OriginCoord,
	int32 Direction) const
{
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	if (!GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}
	Result.Add(OriginCoord);
	return Result;
}
