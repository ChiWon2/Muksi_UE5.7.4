// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/CardRange/Trapezoid/Range_TrapezoidDataAsset.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

TArray<FIntPoint> URange_TrapezoidDataAsset::GetRangeCoords(const ABattleGridManager* GridManager,
                                                            FIntPoint OriginCoord, int32 Direction) const
{
	//return Super::GetRangeCoords(GridManager, OriginCoord, Direction);
	TArray<FIntPoint> Result;

	if (!GridManager || !GridManager->IsValidGridCoord(OriginCoord))
	{
		return Result;
	}

	const FCubeCoord OriginCube = GridManager->OddQToCube(OriginCoord);

	// 방향 인덱스를 0~5 범위로 보정
	const int32 ForwardDirection = ((Direction % 6) + 6) % 6;

	const FCubeCoord ForwardCube =
		GridManager->GetCubeDirection(ForwardDirection);

	/*
	 * 진행 방향 기준 왼쪽으로 뻗는 방향.
	 * GetCubeDirection()이 시계 또는 반시계 순서로 정의되어 있다는 전제다.
	 */
	const FCubeCoord SideCube =
		GridManager->GetCubeDirection((ForwardDirection + 2) % 6);

	constexpr int32 Length = 2;

	/*for (int32 ForwardStep = 1; ForwardStep <= Length; ++ForwardStep)
	{
		/*
		 * 1번째 줄: SideStep = 0~1       → 2칸
		 * 2번째 줄: SideStep = -1~1      → 3칸
		 * 3번째 줄: SideStep = -1~2      → 4칸
		 #1#
		const int32 MinSideStep = -(ForwardStep / 2);
		const int32 MaxSideStep = (ForwardStep + 1) / 2;

		for (int32 SideStep = MinSideStep;
			 SideStep <= MaxSideStep;
			 ++SideStep)
		{
			const FCubeCoord CurrentCube(
				OriginCube.X
					+ ForwardCube.X * ForwardStep
					+ SideCube.X * SideStep,

				OriginCube.Y
					+ ForwardCube.Y * ForwardStep
					+ SideCube.Y * SideStep,

				OriginCube.Z
					+ ForwardCube.Z * ForwardStep
					+ SideCube.Z * SideStep
			);

			const FIntPoint CurrentCoord =
				GridManager->CubeToOddQ(CurrentCube);

			if (!GridManager->IsValidGridCoord(CurrentCoord))
			{
				continue;
			}

			Result.AddUnique(CurrentCoord);
		}
	}*/

	for (int32 ForwardStep = 1; ForwardStep <= Length; ++ForwardStep)
	{
		const int32 HalfWidth = ForwardStep;

		for (int32 SideStep = -HalfWidth;
			 SideStep <= HalfWidth;
			 ++SideStep)
		{
			const FCubeCoord CurrentCube(
				OriginCube.X
					+ ForwardCube.X * ForwardStep
					+ SideCube.X * SideStep,

				OriginCube.Y
					+ ForwardCube.Y * ForwardStep
					+ SideCube.Y * SideStep,

				OriginCube.Z
					+ ForwardCube.Z * ForwardStep
					+ SideCube.Z * SideStep
			);

			const FIntPoint CurrentCoord =
				GridManager->CubeToOddQ(CurrentCube);

			if (GridManager->IsValidGridCoord(CurrentCoord))
			{
				Result.AddUnique(CurrentCoord);
			}
		}
	}
	if (bIncludeOrigin)
	{
		Result.AddUnique(OriginCoord);
	}

	return Result;
}
