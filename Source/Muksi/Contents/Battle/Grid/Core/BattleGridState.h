#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "BattleGridState.generated.h"

/** Grid cell collection과 논리 점유 상태를 담당할 상태 객체. */
UCLASS(BlueprintType)
class MUKSI_API UBattleGridState : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(int32 InWidth, int32 InHeight);
	void Reset();
	bool CopyFrom(const UBattleGridState* Source);
	bool IsValidCoord(const FHexOffsetCoord& Coord) const;
	int32 CoordToIndex(const FHexOffsetCoord& Coord) const;
	FBattleGridCell* GetCell(const FHexOffsetCoord& Coord);
	const FBattleGridCell* GetCell(const FHexOffsetCoord& Coord) const;
	bool SetOccupied(const FHexOffsetCoord& Coord, AActor* Actor);
	bool ClearOccupied(const FHexOffsetCoord& Coord);
	bool MoveOccupant(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord);

	const TArray<FBattleGridCell>& GetCells() const { return Cells; }
	TArray<FBattleGridCell>& GetMutableCells() { return Cells; }
	int32 GetWidth() const { return Width; }
	int32 GetHeight() const { return Height; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Battle|Grid") int32 Width = 0;
	UPROPERTY(VisibleAnywhere, Category = "Battle|Grid") int32 Height = 0;
	UPROPERTY(VisibleAnywhere, Category = "Battle|Grid") TArray<FBattleGridCell> Cells;
};
