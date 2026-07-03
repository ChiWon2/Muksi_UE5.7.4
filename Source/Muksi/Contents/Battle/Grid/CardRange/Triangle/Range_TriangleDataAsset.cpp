// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/Triangle/Range_TriangleDataAsset.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> URange_TriangleDataAsset::GetRangeCoords(const ABattleGridManager* GridManager, FIntPoint OriginCoord,
                                                           int32 Direction) const
{
	TArray<FIntPoint> Result;

	if (!GridManager || !GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}

	const FCubeCoord OriginCube =
		GridManager->OddQToCube(OriginCoord);

	// Direction을 0~5 범위로 보정
	const int32 BaseDirection = ((Direction % 6) + 6) % 6;
	
	Result.AddUnique(OriginCoord);
	/*
	 * 서로 인접한 두 방향을 삼각형의 양쪽 변으로 사용한다.
	 *
	 * EdgeA와 EdgeB의 각도 차이는 60도이다.
	 */
	const FCubeCoord EdgeA =
		GridManager->GetCubeDirection(BaseDirection);

	const FCubeCoord EdgeB =
		GridManager->GetCubeDirection((BaseDirection + 1) % 6);

	constexpr int32 TriangleSize = 3;

	/*
	 * Row 0: OriginCoord 1칸
	 * Row 1: 2칸
	 * Row 2: 3칸
	 */
	for (int32 Row = 0; Row < TriangleSize; ++Row)
	{
		for (int32 EdgeAStep = 0;
			 EdgeAStep <= Row;
			 ++EdgeAStep)
		{
			const int32 EdgeBStep = Row - EdgeAStep;

			const FCubeCoord CurrentCube(
				OriginCube.X
					+ EdgeA.X * EdgeAStep
					+ EdgeB.X * EdgeBStep,

				OriginCube.Y
					+ EdgeA.Y * EdgeAStep
					+ EdgeB.Y * EdgeBStep,

				OriginCube.Z
					+ EdgeA.Z * EdgeAStep
					+ EdgeB.Z * EdgeBStep
			);

			const FIntPoint CurrentCoord =
				GridManager->CubeToOddQ(CurrentCube);

			if (!GridManager->IsValidGridCoord(CurrentCoord))
			{
				continue;
			}

			Result.AddUnique(CurrentCoord);
		}
	}

	return Result;
}
