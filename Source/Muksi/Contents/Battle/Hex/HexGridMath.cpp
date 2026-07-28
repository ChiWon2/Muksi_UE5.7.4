#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

FHexCubeCoord FHexGridMath::OffsetToCube(const FHexOffsetCoord& OffsetCoord)
{
	// Odd-R horizontal layout: odd-numbered rows are shifted right.
	const int32 CubeX = OffsetCoord.X - (OffsetCoord.Y - (OffsetCoord.Y & 1)) / 2;
	const int32 CubeZ = OffsetCoord.Y;
	const int32 CubeY = -CubeX - CubeZ;
	return FHexCubeCoord(CubeX, CubeY, CubeZ);
}

FHexOffsetCoord FHexGridMath::CubeToOffset(const FHexCubeCoord& CubeCoord)
{
	// Odd-R horizontal layout: odd-numbered rows are shifted right.
	const int32 Column = CubeCoord.X + (CubeCoord.Z - (CubeCoord.Z & 1)) / 2;
	const int32 Row = CubeCoord.Z;
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
	FVector2D FlatDirection(WorldDirection.X, WorldDirection.Y);
	if (!FlatDirection.Normalize())
	{
		return INDEX_NONE;
	}

	// Direction indices follow GetCubeDirection():
	// 0 East, 1 North-East, 2 North-West, 3 West, 4 South-West, 5 South-East.
	// Grid rows increase along world +Y, so the north-facing directions use negative Y.
	static const FVector2D DirectionVectors[DirectionCount] =
	{
		FVector2D( 1.0f,  0.0f),
		FVector2D( 0.5f, -0.8660254f),
		FVector2D(-0.5f, -0.8660254f),
		FVector2D(-1.0f,  0.0f),
		FVector2D(-0.5f,  0.8660254f),
		FVector2D( 0.5f,  0.8660254f)
	};

	int32 BestDirection = 0;
	float BestDot = -1.0f;

	for (int32 DirectionIndex = 0; DirectionIndex < DirectionCount; ++DirectionIndex)
	{
		const float Dot = FVector2D::DotProduct(FlatDirection, DirectionVectors[DirectionIndex]);
		if (Dot > BestDot)
		{
			BestDot = Dot;
			BestDirection = DirectionIndex;
		}
	}

	return BestDirection;
}
