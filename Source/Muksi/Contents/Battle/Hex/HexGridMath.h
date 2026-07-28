#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"

class MUKSI_API FHexGridMath
{
public:
	static constexpr int32 DirectionCount = 6;

	static FHexCubeCoord OffsetToCube(const FHexOffsetCoord& OffsetCoord);
	static FHexOffsetCoord CubeToOffset(const FHexCubeCoord& CubeCoord);
	
	static int32 GetCubeDistance(const FHexCubeCoord& A, const FHexCubeCoord& B);
	static int32 GetHexDistance(const FHexOffsetCoord& A, const FHexOffsetCoord& B);
	
	static FHexCubeCoord GetCubeDirection(int32 DirectionIndex);
	static FHexCubeCoord GetNeighborCube(const FHexCubeCoord& OriginCube, int32 DirectionIndex, int32 Distance = 1);
	static FHexOffsetCoord GetNeighborCoord(const FHexOffsetCoord& OriginCoord, int32 DirectionIndex, int32 Distance = 1);

	static int32 NormalizeDirection(int32 DirectionIndex);
	static int32 GetOppositeDirection(int32 DirectionIndex);
	static int32 RotateDirectionLeft(int32 DirectionIndex, int32 StepCount = 1);
	static int32 RotateDirectionRight(int32 DirectionIndex, int32 StepCount = 1);
	static int32 GetClosestDirectionByWorldVector(const FVector& WorldDirection);

	static FHexCubeCoord GetRotateCubeRight60(const FHexCubeCoord& Cube);
	static FHexCubeCoord GetRotateCubeLeft60(const FHexCubeCoord& Cube);
private:
	FHexGridMath() = delete;
	~FHexGridMath() = delete;
};
