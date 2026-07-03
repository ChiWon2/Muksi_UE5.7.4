// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/Hex/MuksiCardRange_Hex2DataAsset.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> UMuksiCardRange_Hex2DataAsset::GetRangeCoords(const ABattleGridManager* GridManager,
                                                                FIntPoint OriginCoord, int32 Direction) const
{
	//return Super::GetRangeCoords(GridManager, OriginCoord, Direction);
	(void)Direction;

	TArray<FIntPoint> Result;

	if (!GridManager || !GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);

	constexpr int32 Range = 2;
	Result.Add(OriginCoord);
	/*
	 * Cube 좌표는 X + Y + Z = 0이어야 하므로
	 * X, Y 변화량을 구한 뒤 Z 변화량을 자동으로 계산한다.
	 */
	for (int32 DeltaX = -Range; DeltaX <= Range; ++DeltaX)
	{
		const int32 MinDeltaY = FMath::Max(-Range, -DeltaX - Range);
		const int32 MaxDeltaY = FMath::Min( Range, -DeltaX + Range);

		for (int32 DeltaY = MinDeltaY; DeltaY <= MaxDeltaY; ++DeltaY)
		{
			const int32 DeltaZ = -DeltaX - DeltaY;

			// 중심을 포함하지 않는 범위라면 건너뛴다.
			if (!bIncludeOrigin &&
				DeltaX == 0 &&
				DeltaY == 0 &&
				DeltaZ == 0)
			{
				continue;
			}

			const FCubeCoord CurrentCube(
				OriginCube.X + DeltaX,
				OriginCube.Y + DeltaY,
				OriginCube.Z + DeltaZ
			);

			const FIntPoint CurrentCoord =
				GridManager->CubeToOddQ(CurrentCube);

			if (!GridManager->IsValidGridCoord(CurrentCoord))
			{
				continue;
			}

			Result.Add(CurrentCoord);
		}
	}
	
	return Result;
}
