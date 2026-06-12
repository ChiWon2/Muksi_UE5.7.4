// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/Line/MuksiCardRange_Line3DataAsset.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> UMuksiCardRange_Line3DataAsset::GetRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord, int32 Direction) const
{
	
	TArray<FIntPoint> Result;
	UE_LOG(LogTemp, Error, TEXT("----------------------------"));
	UE_LOG(LogTemp, Error, TEXT("Select Point X : %d  Y : %d"), OriginCoord.X, OriginCoord.Y);

	if (!GridManager)
	{
		return Result;
	}

	if (!GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);
	const FCubeCoord DirectionCube = GridManager->GetCubeDirection(Direction);

	const int32 StartStep = bIncludeOrigin ? 0 : 1;
	
	const int32 HalfRange = LineLength / 2;
	
	for (int32 Step = StartStep; Step <= HalfRange; ++Step)
	{
		if (Step == 0 && !bIncludeCenter)
		{
			continue;
		}
		const FCubeCoord CurrentCube(
			OriginCube.X + DirectionCube.X * Step,
			OriginCube.Y + DirectionCube.Y * Step,
			OriginCube.Z + DirectionCube.Z * Step
		);

		const FIntPoint CurrentCoord = GridManager->CubeToOddQ(CurrentCube);
		
		UE_LOG(LogTemp, Error, TEXT("cal Point X : %d, Y : %d"), CurrentCoord.X, CurrentCoord.Y);

		if (!GridManager->IsValidGridCoord(CurrentCoord))
		{
			continue;
		}

		Result.Add(CurrentCoord);
	}
	
	for (int32 Step = StartStep; Step <= HalfRange; ++Step)
	{
		if (Step == 0 && !bIncludeCenter)
		{
			continue;
		}
		const FCubeCoord CurrentCube(
			OriginCube.X - DirectionCube.X * Step,
			OriginCube.Y - DirectionCube.Y * Step,
			OriginCube.Z - DirectionCube.Z * Step
		);

		const FIntPoint CurrentCoord = GridManager->CubeToOddQ(CurrentCube);
		
		UE_LOG(LogTemp, Error, TEXT("cal Point X : %d, Y : %d"), CurrentCoord.X, CurrentCoord.Y);

		if (!GridManager->IsValidGridCoord(CurrentCoord))
		{
			continue;
		}

		Result.Add(CurrentCoord);
	}
	Result.Add(OriginCoord);
	
	UE_LOG(LogTemp, Error, TEXT("----------------------------"));
	return Result;
}
