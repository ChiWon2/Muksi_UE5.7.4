#include "Muksi/Contents/Battle/Grid/Core/BattleGridState.h"

void UBattleGridState::Initialize(const int32 InWidth, const int32 InHeight)
{
	Width = FMath::Max(0, InWidth);
	Height = FMath::Max(0, InHeight);
	Cells.Reset();
	Cells.SetNum(Width * Height);
	for (int32 Row = 0; Row < Height; ++Row)
	{
		for (int32 Column = 0; Column < Width; ++Column)
		{
			Cells[Row * Width + Column].GridCoord = FHexOffsetCoord(Column, Row);
		}
	}
}

void UBattleGridState::Reset() { Width = 0; Height = 0; Cells.Reset(); }
bool UBattleGridState::CopyFrom(const UBattleGridState* Source)
{
	if (!IsValid(Source)) return false;
	Width = Source->Width; Height = Source->Height; Cells = Source->Cells; return true;
}
bool UBattleGridState::IsValidCoord(const FHexOffsetCoord& Coord) const { return Coord.X >= 0 && Coord.X < Width && Coord.Y >= 0 && Coord.Y < Height; }
int32 UBattleGridState::CoordToIndex(const FHexOffsetCoord& Coord) const { return IsValidCoord(Coord) ? Coord.Y * Width + Coord.X : INDEX_NONE; }
FBattleGridCell* UBattleGridState::GetCell(const FHexOffsetCoord& Coord) { const int32 I=CoordToIndex(Coord); return Cells.IsValidIndex(I) ? &Cells[I] : nullptr; }
const FBattleGridCell* UBattleGridState::GetCell(const FHexOffsetCoord& Coord) const { const int32 I=CoordToIndex(Coord); return Cells.IsValidIndex(I) ? &Cells[I] : nullptr; }
bool UBattleGridState::SetOccupied(const FHexOffsetCoord& Coord, AActor* Actor)
{
	FBattleGridCell* Cell=GetCell(Coord); if (!Cell || !IsValid(Actor) || !Cell->bWalkable || Cell->bOccupied) return false;
	Cell->bOccupied=true; Cell->OccupyingActor=Actor; return true;
}
bool UBattleGridState::ClearOccupied(const FHexOffsetCoord& Coord)
{
	FBattleGridCell* Cell=GetCell(Coord); if (!Cell) return false; Cell->bOccupied=false; Cell->OccupyingActor=nullptr; return true;
}
bool UBattleGridState::MoveOccupant(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord)
{
	FBattleGridCell* From=GetCell(FromCoord); FBattleGridCell* To=GetCell(ToCoord);
	if (!From || !To || !IsValid(Actor) || From->OccupyingActor != Actor || !To->bWalkable || To->bOccupied) return false;
	From->bOccupied=false; From->OccupyingActor=nullptr; To->bOccupied=true; To->OccupyingActor=Actor; return true;
}
