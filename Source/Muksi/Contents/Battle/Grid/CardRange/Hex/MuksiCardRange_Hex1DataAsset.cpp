// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/Hex/MuksiCardRange_Hex1DataAsset.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> UMuksiCardRange_Hex1DataAsset::GetRangeCoords(const ABattleGridManager* GridManager,
                                                                FIntPoint OriginCoord, int32 Direction) const
{
	//return Super::GetRangeCoords(GridManager, OriginCoord, Direction);
	TArray<FIntPoint> Result;

	if (!GridManager)
	{
		return Result;
	}

	if (!GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);

	// 중심 좌표 포함 여부
	if (bIncludeOrigin)
	{
		Result.Add(OriginCoord);
	}

	// 중심을 둘러싼 6방향의 인접 타일
	for (int32 DirectionIndex = 0; DirectionIndex < 6; ++DirectionIndex)
	{
		const FCubeCoord DirectionCube =
			GridManager->GetCubeDirection(DirectionIndex);

		const FCubeCoord CurrentCube(
			OriginCube.X + DirectionCube.X,
			OriginCube.Y + DirectionCube.Y,
			OriginCube.Z + DirectionCube.Z
		);

		const FIntPoint CurrentCoord =
			GridManager->CubeToOddQ(CurrentCube);

		if (!GridManager->IsValidGridCoord(CurrentCoord))
		{
			continue;
		}

		Result.Add(CurrentCoord);
	}
	Result.Add(OriginCoord);
	return Result;
}
