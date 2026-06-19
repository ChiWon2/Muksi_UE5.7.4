// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> UMuksiCardRangeDataAsset::GetRangeCoords(const ABattleGridManager* GridManager, FIntPoint OriginCoord,
	int32 Direction) const
{
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);
	const FCubeCoord DirectionCube = GridManager->GetCubeDirection(Direction);

	const int32 StartStep = bIncludeOrigin ? 0 : 1;
	
	constexpr int32 Range = 1;
	
	for (int32 Step = StartStep; Step <= Range; ++Step)
	{
		const FCubeCoord CurrentCube(
			OriginCube.X + DirectionCube.X * Step,
			OriginCube.Y + DirectionCube.Y * Step,
			OriginCube.Z + DirectionCube.Z * Step
		);

		const FIntPoint CurrentCoord = GridManager->CubeToOddQ(CurrentCube);

		if (!GridManager->IsValidGridCoord(CurrentCoord))
		{
			break;
		}

		Result.Add(CurrentCoord);
	}

	return Result;
}

TArray<FIntPoint> UMuksiCardRangeDataAsset::GetLineRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord, int32 Direction) const
{
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);
	const FCubeCoord DirectionCube = GridManager->GetCubeDirection(Direction);

	const int32 StartStep = bIncludeOrigin ? 0 : 1;

	constexpr int32 Range = 1;
	
	for (int32 Step = StartStep; Step <= Range; ++Step)
	{
		const FCubeCoord CurrentCube(
			OriginCube.X + DirectionCube.X * Step,
			OriginCube.Y + DirectionCube.Y * Step,
			OriginCube.Z + DirectionCube.Z * Step
		);

		const FIntPoint CurrentCoord = GridManager->CubeToOddQ(CurrentCube);

		if (!GridManager->IsValidGridCoord(CurrentCoord))
		{
			break;
		}

		Result.Add(CurrentCoord);
	}

	return Result;
}

TArray<FIntPoint> UMuksiCardRangeDataAsset::GetAroundRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord) const
{
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);

	constexpr int32 Range = 1;
	
	for (int32 DX = -Range; DX <= Range; ++DX)
	{
		for (int32 DY = FMath::Max(-Range, -DX - Range);
			 DY <= FMath::Min(Range, -DX + Range);
			 ++DY)
		{
			const int32 DZ = -DX - DY;

			const FCubeCoord CurrentCube(
				OriginCube.X + DX,
				OriginCube.Y + DY,
				OriginCube.Z + DZ
			);

			const FIntPoint CurrentCoord = GridManager->CubeToOddQ(CurrentCube);

			if (!GridManager->IsValidGridCoord(CurrentCoord))
			{
				continue;
			}

			if (!bIncludeOrigin && CurrentCoord == OriginCoord)
			{
				continue;
			}

			Result.Add(CurrentCoord);
		}
	}

	return Result;
}

TArray<FIntPoint> UMuksiCardRangeDataAsset::GetCustomPatternRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord, int32 Direction) const
{
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);

	Direction = ((Direction % 6) + 6) % 6;

	for (const FIntPoint& PatternCoord : PatternCoords)
	{
		FCubeCoord RelativeCube = GridManager->OddQToCube(PatternCoord);

		for (int32 i = 0; i < Direction; ++i)
		{
			// 시계 방향 60도 회전
			RelativeCube = FCubeCoord(
				-RelativeCube.Z,
				-RelativeCube.X,
				-RelativeCube.Y
			);
		}

		const FCubeCoord FinalCube(
			OriginCube.X + RelativeCube.X,
			OriginCube.Y + RelativeCube.Y,
			OriginCube.Z + RelativeCube.Z
		);

		const FIntPoint FinalCoord = GridManager->CubeToOddQ(FinalCube);

		if (!GridManager->IsValidGridCoord(FinalCoord))
		{
			continue;
		}

		if (!bIncludeOrigin && FinalCoord == OriginCoord)
		{
			continue;
		}

		Result.Add(FinalCoord);
	}

	return Result;
}
