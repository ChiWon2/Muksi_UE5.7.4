#pragma once

#include "CoreMinimal.h"
#include "HexGridCoord.generated.h"

USTRUCT(BlueprintType)
struct FCubeCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Grid")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Grid")
	int32 Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Grid")
	int32 Z = 0;

	FCubeCoord() = default;

	FCubeCoord(int32 InX, int32 InY, int32 InZ)
		: X(InX)
		, Y(InY)
		, Z(InZ)
	{
	}

	bool operator==(const FCubeCoord& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}

	bool operator!=(const FCubeCoord& Other) const
	{
		return !(*this == Other);
	}

	FCubeCoord operator+(const FCubeCoord& Other) const
	{
		return FCubeCoord(X + Other.X, Y + Other.Y, Z + Other.Z);
	}

	FCubeCoord operator-(const FCubeCoord& Other) const
	{
		return FCubeCoord(X - Other.X, Y - Other.Y, Z - Other.Z);
	}

	FCubeCoord operator*(int32 Scalar) const
	{
		return FCubeCoord(X * Scalar, Y * Scalar, Z * Scalar);
	}

	bool IsValidCubeCoord() const
	{
		return X + Y + Z == 0;
	}
};