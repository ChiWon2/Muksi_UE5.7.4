#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Grid/Hex/HexGridCoord.h"

class MUKSI_API FHexGridMath
{
public:

	static constexpr int32 DirectionCount = 6;

	// Odd-Q Offset 좌표를 Cube 좌표로 변환한다.
	static FCubeCoord OddQToCube(const FIntPoint& GridCoord);

	// Cube 좌표를 Odd-Q Offset 좌표로 변환한다.
	static FIntPoint CubeToOddQ(const FCubeCoord& CubeCoord);

	// 두 Cube 좌표 사이의 Hex 거리를 반환한다.
	static int32 GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B);

	// 두 Grid 좌표 사이의 Hex 거리를 반환한다.
	static int32 GetHexDistance(const FIntPoint& A, const FIntPoint& B);

	// 0~5 방향에 대응하는 Cube 방향 좌표를 반환한다.
	static FCubeCoord GetCubeDirection(int32 DirectionIndex);

	// Origin Cube 좌표에서 지정한 방향과 거리만큼 이동한 Cube 좌표를 반환한다.
	static FCubeCoord GetNeighborCube(const FCubeCoord& OriginCube, int32 DirectionIndex, int32 Distance = 1);

	// Origin Grid 좌표에서 지정한 방향과 거리만큼 이동한 Grid 좌표를 반환한다.
	static FIntPoint GetNeighborCoord(const FIntPoint& OriginCoord, int32 DirectionIndex, int32 Distance = 1);

	// 방향 인덱스를 0~5 범위로 정규화한다.
	static int32 NormalizeDirection(int32 DirectionIndex);

	// 반대 방향을 반환한다.
	static int32 GetOppositeDirection(int32 DirectionIndex);

	// 왼쪽으로 회전한 방향을 반환한다.
	static int32 RotateDirectionLeft(int32 DirectionIndex, int32 StepCount = 1);

	// 오른쪽으로 회전한 방향을 반환한다.
	static int32 RotateDirectionRight(int32 DirectionIndex, int32 StepCount = 1);

	// 월드 평면 방향을 가장 가까운 Hex 방향으로 변환한다.
	static int32 GetClosestDirectionByWorldVector(const FVector& WorldDirection);

private:

	FHexGridMath() = delete;
	~FHexGridMath() = delete;
};