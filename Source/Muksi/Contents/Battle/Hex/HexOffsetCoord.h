#pragma once

#include "CoreMinimal.h"
#include "HexOffsetCoord.generated.h"

/**
 *
 * Offset coordinates : Odd-Q Vertical Offset
 *
 * X = Column
 * Y = Row
 */
USTRUCT(BlueprintType)
struct MUKSI_API FHexOffsetCoord
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
	int32 Y = 0;

public:
	FHexOffsetCoord() = default;

	FHexOffsetCoord(const int32 InColumn, const int32 InRow)
		: X(InColumn), Y(InRow)
	{
	}

	FHexOffsetCoord(const FHexOffsetCoord& Other) = default;

	FHexOffsetCoord(FHexOffsetCoord&& Other) noexcept = default;

	FHexOffsetCoord& operator=(const FHexOffsetCoord& Other) = default;

	FHexOffsetCoord& operator=(FHexOffsetCoord&& Other) noexcept = default;

	explicit FHexOffsetCoord(const FIntPoint& Point)
		: X(Point.X), Y(Point.Y)
	{
	}

public:
	FORCEINLINE int32 GetColumn() const
	{
		return X;
	}

	FORCEINLINE int32 GetRow() const
	{
		return Y;
	}

	FORCEINLINE FIntPoint ToIntPoint() const
	{
		return FIntPoint(X, Y);
	}

	FORCEINLINE bool IsValid() const
	{
		return X != INDEX_NONE && Y != INDEX_NONE;
	}

	FORCEINLINE bool IsInvalid() const
	{
		return !IsValid();
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Column=%d Row=%d"), X, Y);
	}

public:
	FORCEINLINE bool operator==(const FHexOffsetCoord& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	FORCEINLINE bool operator!=(const FHexOffsetCoord& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE FHexOffsetCoord operator+(const FHexOffsetCoord& Other) const
	{
		return FHexOffsetCoord(X + Other.X, Y + Other.Y);
	}

	FORCEINLINE FHexOffsetCoord operator-(const FHexOffsetCoord& Other) const
	{
		return FHexOffsetCoord(X - Other.X, Y - Other.Y);
	}

	FORCEINLINE FHexOffsetCoord& operator+=(const FHexOffsetCoord& Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}

	FORCEINLINE FHexOffsetCoord& operator-=(const FHexOffsetCoord& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}

public:
	static FORCEINLINE FHexOffsetCoord Zero()
	{
		return FHexOffsetCoord(0, 0);
	}

	static FORCEINLINE FHexOffsetCoord Invalid()
	{
		return FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	}
};

FORCEINLINE uint32 GetTypeHash(const FHexOffsetCoord& Coord)
{
	return HashCombine(::GetTypeHash(Coord.X), ::GetTypeHash(Coord.Y));
}