#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

FHexCubeCoord FHexGridMath::OffsetToCube(const FHexOffsetCoord& OffsetCoord)
{
	const int32 CubeX = OffsetCoord.X;
	const int32 CubeZ = OffsetCoord.Y - (OffsetCoord.X - (OffsetCoord.X & 1)) / 2;
	const int32 CubeY = -CubeX - CubeZ;
	return FHexCubeCoord(CubeX, CubeY, CubeZ);
}

FHexOffsetCoord FHexGridMath::CubeToOffset(const FHexCubeCoord& CubeCoord)
{
	const int32 Column = CubeCoord.X;
	const int32 Row = CubeCoord.Z + (CubeCoord.X - (CubeCoord.X & 1)) / 2;
	return FHexOffsetCoord(Column, Row);
}

int32 FHexGridMath::GetCubeDistance(const FHexCubeCoord& A, const FHexCubeCoord& B)
{
	return FMath::Max3(FMath::Abs(A.X - B.X), FMath::Abs(A.Y - B.Y), FMath::Abs(A.Z - B.Z));
}

int32 FHexGridMath::GetHexDistance(const FHexOffsetCoord& A, const FHexOffsetCoord& B)
{
	return GetCubeDistance(OffsetToCube(A), OffsetToCube(B));
}

FHexCubeCoord FHexGridMath::GetCubeDirection(const int32 DirectionIndex)
{
	static const FHexCubeCoord Directions[DirectionCount] =
	{
		{1, -1, 0}, {1, 0, -1}, {0, 1, -1},
		{-1, 1, 0}, {-1, 0, 1}, {0, -1, 1}
	};
	return Directions[NormalizeDirection(DirectionIndex)];
}

FHexCubeCoord FHexGridMath::GetNeighborCube(const FHexCubeCoord& OriginCube, const int32 DirectionIndex, const int32 Distance)
{
	return OriginCube + GetCubeDirection(DirectionIndex) * FMath::Max(0, Distance);
}

FHexOffsetCoord FHexGridMath::GetNeighborCoord(const FHexOffsetCoord& OriginCoord, const int32 DirectionIndex, const int32 Distance)
{
	return CubeToOffset(GetNeighborCube(OffsetToCube(OriginCoord), DirectionIndex, Distance));
}

int32 FHexGridMath::NormalizeDirection(const int32 DirectionIndex)
{
	int32 Result = DirectionIndex % DirectionCount;
	if (Result < 0) Result += DirectionCount;
	return Result;
}

int32 FHexGridMath::GetOppositeDirection(const int32 DirectionIndex) { return NormalizeDirection(DirectionIndex + DirectionCount / 2); }
int32 FHexGridMath::RotateDirectionLeft(const int32 DirectionIndex, const int32 StepCount) { return NormalizeDirection(DirectionIndex - StepCount); }
int32 FHexGridMath::RotateDirectionRight(const int32 DirectionIndex, const int32 StepCount) { return NormalizeDirection(DirectionIndex + StepCount); }

int32 FHexGridMath::GetClosestDirectionByWorldVector(const FVector& WorldDirection)
{
	FVector FlatDirection = WorldDirection;
	FlatDirection.Z = 0.0f;
	if (!FlatDirection.Normalize()) return INDEX_NONE;
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(FlatDirection.Y, FlatDirection.X));
	if (Angle < 0.0f) Angle += 360.0f;
	return NormalizeDirection(FMath::RoundToInt(Angle / 60.0f));
}
