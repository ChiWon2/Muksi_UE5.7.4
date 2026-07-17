#include "Muksi/Contents/Battle/Grid/Hex/HexGridMath.h"

FCubeCoord FHexGridMath::OddQToCube(const FIntPoint& GridCoord)
{
	const int32 CubeX = GridCoord.X;
	const int32 CubeZ = GridCoord.Y - (GridCoord.X - (GridCoord.X & 1)) / 2;
	const int32 CubeY = -CubeX - CubeZ;

	return FCubeCoord(CubeX, CubeY, CubeZ);
}

FIntPoint FHexGridMath::CubeToOddQ(const FCubeCoord& CubeCoord)
{
	const int32 GridX = CubeCoord.X;
	const int32 GridY = CubeCoord.Z + (CubeCoord.X - (CubeCoord.X & 1)) / 2;

	return FIntPoint(GridX, GridY);
}

int32 FHexGridMath::GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B)
{
	const int32 DeltaX = FMath::Abs(A.X - B.X);
	const int32 DeltaY = FMath::Abs(A.Y - B.Y);
	const int32 DeltaZ = FMath::Abs(A.Z - B.Z);

	return FMath::Max3(DeltaX, DeltaY, DeltaZ);
}

int32 FHexGridMath::GetHexDistance(const FIntPoint& A, const FIntPoint& B)
{
	return GetCubeDistance(OddQToCube(A), OddQToCube(B));
}

FCubeCoord FHexGridMath::GetCubeDirection(int32 DirectionIndex)
{
	static const FCubeCoord CubeDirections[DirectionCount] =
	{
		FCubeCoord(1, -1, 0),
		FCubeCoord(1, 0, -1),
		FCubeCoord(0, 1, -1),
		FCubeCoord(-1, 1, 0),
		FCubeCoord(-1, 0, 1),
		FCubeCoord(0, -1, 1)
	};

	return CubeDirections[NormalizeDirection(DirectionIndex)];
}

FCubeCoord FHexGridMath::GetNeighborCube(const FCubeCoord& OriginCube, int32 DirectionIndex, int32 Distance)
{
	return OriginCube + GetCubeDirection(DirectionIndex) * FMath::Max(0, Distance);
}

FIntPoint FHexGridMath::GetNeighborCoord(const FIntPoint& OriginCoord, int32 DirectionIndex, int32 Distance)
{
	return CubeToOddQ(GetNeighborCube(OddQToCube(OriginCoord), DirectionIndex, Distance));
}

int32 FHexGridMath::NormalizeDirection(int32 DirectionIndex)
{
	int32 Result = DirectionIndex % DirectionCount;

	if (Result < 0)
	{
		Result += DirectionCount;
	}

	return Result;
}

int32 FHexGridMath::GetOppositeDirection(int32 DirectionIndex)
{
	return NormalizeDirection(DirectionIndex + DirectionCount / 2);
}

int32 FHexGridMath::RotateDirectionLeft(int32 DirectionIndex, int32 StepCount)
{
	return NormalizeDirection(DirectionIndex - StepCount);
}

int32 FHexGridMath::RotateDirectionRight(int32 DirectionIndex, int32 StepCount)
{
	return NormalizeDirection(DirectionIndex + StepCount);
}

int32 FHexGridMath::GetClosestDirectionByWorldVector(const FVector& WorldDirection)
{
	FVector FlatDirection = WorldDirection;
	FlatDirection.Z = 0.0f;

	if (!FlatDirection.Normalize())
	{
		return INDEX_NONE;
	}

	float Angle = FMath::RadiansToDegrees(FMath::Atan2(FlatDirection.Y, FlatDirection.X));

	if (Angle < 0.0f)
	{
		Angle += 360.0f;
	}

	return NormalizeDirection(FMath::RoundToInt(Angle / 60.0f));
}