#pragma once

#include "CoreMinimal.h"
#include "HexCubeCoord.generated.h"

/** Hex 계산용 cube coordinate. X + Y + Z == 0 이어야 한다. */
USTRUCT(BlueprintType)
struct MUKSI_API FHexCubeCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 Z = 0;

	FHexCubeCoord() = default;
	FHexCubeCoord(const int32 InX, const int32 InY, const int32 InZ)
		: X(InX), Y(InY), Z(InZ)
	{
	}

	bool operator==(const FHexCubeCoord& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}

	bool operator!=(const FHexCubeCoord& Other) const { return !(*this == Other); }
	FHexCubeCoord operator+(const FHexCubeCoord& Other) const { return {X + Other.X, Y + Other.Y, Z + Other.Z}; }
	FHexCubeCoord operator-(const FHexCubeCoord& Other) const { return {X - Other.X, Y - Other.Y, Z - Other.Z}; }
	FHexCubeCoord operator*(const int32 Scalar) const { return {X * Scalar, Y * Scalar, Z * Scalar}; }
	bool IsValidCubeCoord() const { return X + Y + Z == 0; }
};

FORCEINLINE uint32 GetTypeHash(const FHexCubeCoord& Coord)
{
	return HashCombine(HashCombine(::GetTypeHash(Coord.X), ::GetTypeHash(Coord.Y)), ::GetTypeHash(Coord.Z));
}
