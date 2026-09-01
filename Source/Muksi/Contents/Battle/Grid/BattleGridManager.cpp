#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"

#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Grid/Generator/BattleGridTileGeneratorComponent.h"
#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

ABattleGridManager::ABattleGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	BattleGridGeneratorComponent = CreateDefaultSubobject<UBattleGridTileGeneratorComponent>(TEXT("BattleGridGeneratorComponent"));
	BattleGridIndicatorComponent = CreateDefaultSubobject<UBattleGridIndicatorComponent>(TEXT("BattleGridIndicatorComponent"));
	BattleGridNavigationComponent = CreateDefaultSubobject<UBattleGridNavigationComponent>(TEXT("BattleGridNavigationComponent"));
}

void ABattleGridManager::BeginPlay()
{
	Super::BeginPlay();

	GenerateGrid();
}

void ABattleGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GridStates.Reset();
	Super::EndPlay(EndPlayReason);
}

void ABattleGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

bool ABattleGridManager::IsValidCoord(const FHexOffsetCoord& Coord) const
{
	const FBattleGridLayoutSettings& Layout = GetLayoutSettings();
	return Coord.X >= 0 && Coord.X < Layout.GridWidth && Coord.Y >= 0 && Coord.Y < Layout.GridHeight;
}

ABattleGridTile* ABattleGridManager::GetTileActorByCoord(const FHexOffsetCoord& Coord) const
{
	const FBattleGridCell* Cell = GetCellByCoord(EBattleSimulationWorldType::PlayerActualEnemyActual, Coord);
	return Cell ? Cell->TileActor.Get() : nullptr;
}

const FBattleGridLayoutSettings& ABattleGridManager::GetLayoutSettings() const
{
	check(BattleGridGeneratorComponent);
	return BattleGridGeneratorComponent->GetLayoutSettings();
}

//Test Hex Cell Dir Cal
void ABattleGridManager::PlaceCharacter(EBattleSimulationWorldType WorldType, ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& Coord)
{
	if (!CharacterBase)
	{
		return;
	}

	const FBattleGridCell* Cell = GetCellByCoord(WorldType, Coord);

	if (!Cell || !Cell->TileActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Move Character Index = %d"), CoordToIndex(Coord));

	CharacterBase->SetActorTransform(Cell->TileActor->GetCharacterSpawnTransform());
	CharacterBase->SetCharacterPosition(Coord);
	SetOccupied(WorldType, Coord, CharacterBase);
}

void ABattleGridManager::GenerateGrid()
{
	if (BattleGridGeneratorComponent)
	{
		BattleGridGeneratorComponent->GenerateTiles();
	}
	for (auto It = GridStates.CreateIterator(); It; ++It)
	{
		if (It.Key() != EBattleSimulationWorldType::PlayerActualEnemyActual) It.RemoveCurrent();
	}
}

void ABattleGridManager::ClearGrid()
{
	if (BattleGridGeneratorComponent)
	{
		BattleGridGeneratorComponent->ClearTiles();
	}
	GridStates.Reset();
}

int32 ABattleGridManager::CoordToIndex(const FHexOffsetCoord& Coord) const
{
	return Coord.Y * GetLayoutSettings().GridWidth + Coord.X;
}

FVector ABattleGridManager::GetWorldLocationByCoord(const FHexOffsetCoord& Coord) const
{
	// Flat-top hex + Odd-R horizontal offset.
	// Y가 홀수인 행은 X 방향으로 반 칸 이동한다.

	const FBattleGridLayoutSettings& Layout = GetLayoutSettings();
	const float LocalX = Layout.GridSpacingX * (Coord.X + Layout.OddRowXOffsetRatio * (Coord.Y & 1));
	const float LocalY = Layout.GridSpacingY * Coord.Y;
	const FVector LocalLocation(LocalX, LocalY, 0.0f);

	return GetActorTransform().TransformPosition(LocalLocation);
}

bool ABattleGridManager::GetPresentationWorldLocationByCoord(const FHexOffsetCoord& Coord, FVector& OutWorldLocation) const
{
	OutWorldLocation = FVector::ZeroVector;
	if (!IsValidCoord(Coord)) return false;
	const int32 Index = CoordToIndex(Coord);
	const TArray<FBattleGridCell>& ActualCells = GetGridCells(EBattleSimulationWorldType::PlayerActualEnemyActual);
	if (!ActualCells.IsValidIndex(Index)) return false;
	OutWorldLocation = ActualCells[Index].WorldLocation;
	return true;
}

bool ABattleGridManager::GetPresentationCoordFromHit(const FHitResult& HitResult, FHexOffsetCoord& OutCoord) const
{
	OutCoord = FHexOffsetCoord::Invalid();

	AActor* HitActor = HitResult.GetActor();
	if (const ABattleGridTile* HitTile = Cast<ABattleGridTile>(HitActor))
	{
		OutCoord = HitTile->GetGridCoord();
		return OutCoord.IsValid();
	}

	if (const ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitActor))
	{
		OutCoord = HitCharacter->GetCharacterCoord();
		return OutCoord.IsValid();
	}

	float BestDistanceSquared = TNumericLimits<float>::Max();
	for (int32 X = 0; X < GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord Coord(X, Y);
			FVector PresentationLocation = FVector::ZeroVector;
			if (!GetPresentationWorldLocationByCoord(Coord, PresentationLocation))
			{
				continue;
			}

			const FVector Delta = PresentationLocation - HitResult.ImpactPoint;
			const float DistanceSquared = FVector2D(Delta.X, Delta.Y).SizeSquared();
			if (DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				OutCoord = Coord;
			}
		}
	}

	return OutCoord.IsValid();
}

float ABattleGridManager::GetAdjacentTileCenterDistance()
{
	float MinimumDistance = TNumericLimits<float>::Max();

	for (int32 X = 0; X < GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CellCoord(X, Y);
			const FVector CellLocation = GetWorldLocationByCoord(CellCoord);
			const TArray<FHexOffsetCoord> NeighborCoords = GetHexNeighbors(CellCoord);

			for (const FHexOffsetCoord& NeighborCoord : NeighborCoords)
			{
				const FVector NeighborLocation = GetWorldLocationByCoord(NeighborCoord);
				const float Distance = FVector::Dist2D(CellLocation, NeighborLocation);

				if (Distance <= KINDA_SMALL_NUMBER)
				{
					continue;
				}

				MinimumDistance = FMath::Min(MinimumDistance, Distance);
			}
		}
	}

	if (MinimumDistance != TNumericLimits<float>::Max())
	{
		return MinimumDistance;
	}

	const FBattleGridLayoutSettings& Layout = GetLayoutSettings();
	const float DiagonalDistance = FMath::Sqrt(FMath::Square(Layout.GridSpacingX * Layout.OddRowXOffsetRatio) + FMath::Square(Layout.GridSpacingY));

	return FMath::Min(Layout.GridSpacingX, DiagonalDistance);
}

float ABattleGridManager::GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius)
{
	const int32 SafeGridRange = FMath::Max(0, GridRange);
	float WorldRadius = GetAdjacentTileCenterDistance() * SafeGridRange;

	if (bIncludeOuterTileRadius)
	{
		WorldRadius += GetLayoutSettings().HexRadius;
	}

	return WorldRadius;
}

const FBattleGridCell* ABattleGridManager::GetCellByCoord(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord) const
{
	if (!IsValidCoord(Coord))
	{
		return nullptr;
	}

	const TArray<FBattleGridCell>& Cells = GetGridCells(WorldType);
	const int32 Index = CoordToIndex(Coord);
	return Cells.IsValidIndex(Index) ? &Cells[Index] : nullptr;
}

void ABattleGridManager::GetCharactersAtCoords(EBattleSimulationWorldType WorldType, const TArray<FHexOffsetCoord>& Coords, TArray<TObjectPtr<ABattleCharacterBase>>& OutCharacters) const
{
	OutCharacters.Reset();
	for (const FHexOffsetCoord& Coord : Coords)
	{
		const FBattleGridCell* Cell = GetCellByCoord(WorldType, Coord);
		ABattleCharacterBase* Character = Cell ? Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()) : nullptr;
		if (IsValid(Character)) OutCharacters.AddUnique(Character);
	}
}


FBattleGridCell* ABattleGridManager::GetMutableCellByCoord(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord)
{
	if (!IsValidCoord(Coord)) return nullptr;
	TArray<FBattleGridCell>& Cells = GetMutableGridCells(WorldType);
	const int32 Index = CoordToIndex(Coord);
	return Cells.IsValidIndex(Index) ? &Cells[Index] : nullptr;
}

TArray<FHexOffsetCoord> ABattleGridManager::GetHexNeighbors(const FHexOffsetCoord& Coord) const
{
	TArray<FHexOffsetCoord> Neighbors;

	if (!IsValidCoord(Coord))
	{
		return Neighbors;
	}

	Neighbors.Reserve(FHexGridMath::DirectionCount);

	for (int32 DirectionIndex = 0; DirectionIndex < FHexGridMath::DirectionCount; ++DirectionIndex)
	{
		const FHexOffsetCoord NeighborCoord = FHexGridMath::GetNeighborCoord(Coord, DirectionIndex);

		if (IsValidCoord(NeighborCoord))
		{
			Neighbors.Add(NeighborCoord);
		}
	}

	return Neighbors;
}

TArray<FHexOffsetCoord> ABattleGridManager::GetMovableCoords(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& StartCoord, int32 MoveRange)
{
	TArray<FHexOffsetCoord> Result;

	if (!IsValidCoord(StartCoord))
	{
		return Result;
	}

	if (MoveRange <= 0)
	{
		return Result;
	}

	TQueue<TPair<FHexOffsetCoord, int32>> Queue;
	TSet<FHexOffsetCoord> Visited;

	Queue.Enqueue(TPair<FHexOffsetCoord, int32>(StartCoord, 0));
	Visited.Add(StartCoord);

	while (!Queue.IsEmpty())
	{
		TPair<FHexOffsetCoord, int32> Current;

		Queue.Dequeue(Current);

		const FHexOffsetCoord CurrentCoord = Current.Key;
		const int32 CurrentDistance = Current.Value;

		if (CurrentDistance > 0)
		{
			Result.Add(CurrentCoord);
		}

		if (CurrentDistance >= MoveRange)
		{
			continue;
		}

		const TArray<FHexOffsetCoord> Neighbors = GetHexNeighbors(CurrentCoord);

		for (const FHexOffsetCoord& NextCoord : Neighbors)
		{
			if (Visited.Contains(NextCoord))
			{
				continue;
			}

			const FBattleGridCell* NextCell = GetCellByCoord(WorldType, NextCoord);

			if (!NextCell)
			{
				continue;
			}

			if (!NextCell->bWalkable)
			{
				continue;
			}

			if (NextCell->bOccupied)
			{
				continue;
			}

			Visited.Add(NextCoord);
			Queue.Enqueue(TPair<FHexOffsetCoord, int32>(NextCoord, CurrentDistance + 1));
		}
	}

	return Result;
}

bool ABattleGridManager::SetOccupied(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord, AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	FBattleGridCell* DestinationCell = GetMutableCellByCoord(WorldType, Coord);
	if (!DestinationCell || !DestinationCell->bWalkable || DestinationCell->bOccupied)
	{
		return false;
	}

	// 같은 Actor가 이미 다른 셀에 있다면 기존 점유를 해제한다.
	for (FBattleGridCell& Cell : GetMutableGridCells(WorldType))
	{
		if (Cell.OccupyingActor == Actor)
		{
			Cell.OccupyingActor = nullptr;
			Cell.bOccupied = false;
			break;
		}
	}

	DestinationCell->OccupyingActor = Actor;
	DestinationCell->bOccupied = true;
	return true;
}

bool ABattleGridManager::ClearOccupied(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord)
{
	FBattleGridCell* Cell = GetMutableCellByCoord(WorldType, Coord);
	if (!Cell)
	{
		return false;
	}

	Cell->OccupyingActor = nullptr;
	Cell->bOccupied = false;
	return true;
}

FBattleGridMoveResult ABattleGridManager::ExecuteGridMove(const FBattleGridMoveRequest& Request)
{
	FBattleGridMoveResult Result;
	Result.FinalCoord = Request.FromCoord;

	ABattleCharacterBase* Character = Request.Character.Get();
	if (!IsValid(Character) || Request.FromCoord == Request.ToCoord)
	{
		return Result;
	}

	FBattleGridCell* FromCell = GetMutableCellByCoord(Request.WorldType, Request.FromCoord);
	FBattleGridCell* ToCell = GetMutableCellByCoord(Request.WorldType, Request.ToCoord);
	if (!FromCell || !ToCell || FromCell->OccupyingActor != Character || !ToCell->bWalkable || ToCell->bOccupied)
	{
		return Result;
	}

	FromCell->OccupyingActor = nullptr;
	FromCell->bOccupied = false;
	ToCell->OccupyingActor = Character;
	ToCell->bOccupied = true;
	Character->SetCharacterPosition(Request.ToCoord);

	if (Request.bSnapActorToGrid)
	{
		Character->SetActorTransform(GetTransformToPosition(Request.ToCoord));
	}

	Result.bSucceeded = true;
	Result.FinalCoord = Request.ToCoord;
	return Result;
}

bool ABattleGridManager::MoveActorOnGrid(EBattleSimulationWorldType WorldType, AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord)
{
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(Actor))
	{
		FBattleGridMoveRequest Request;
		Request.Character = Character;
		Request.WorldType = WorldType;
		Request.FromCoord = FromCoord;
		Request.ToCoord = ToCoord;
		Request.bSnapActorToGrid = true;
		return ExecuteGridMove(Request).bSucceeded;
	}

	if (!IsValid(Actor) || FromCoord == ToCoord)
	{
		return false;
	}

	FBattleGridCell* FromCell = GetMutableCellByCoord(WorldType, FromCoord);
	FBattleGridCell* ToCell = GetMutableCellByCoord(WorldType, ToCoord);

	if (!FromCell || !ToCell)
	{
		return false;
	}

	if (FromCell->OccupyingActor != Actor || !ToCell->bWalkable || ToCell->bOccupied)
	{
		return false;
	}

	FromCell->OccupyingActor = nullptr;
	FromCell->bOccupied = false;
	ToCell->OccupyingActor = Actor;
	ToCell->bOccupied = true;
	Actor->SetActorLocation(GetTransformToPosition(ToCoord).GetLocation());
	return true;
}

FTransform ABattleGridManager::GetTransformToPosition(const FHexOffsetCoord& InPosition)
{
	const FBattleGridCell* Cell = GetCellByCoord(EBattleSimulationWorldType::PlayerActualEnemyActual, InPosition);

	if (!Cell || !Cell->TileActor)
	{
		return FTransform::Identity;
	}

	return Cell->TileActor->GetCharacterSpawnTransform();
}

bool ABattleGridManager::CheckGridInRange(const FHexOffsetCoord& A, const FHexOffsetCoord& B, int32 Range)
{
	if (!IsValidCoord(A) || !IsValidCoord(B) || Range < 0)
	{
		return false;
	}

	const FHexCubeCoord CubeA = FHexGridMath::OffsetToCube(A);
	const FHexCubeCoord CubeB = FHexGridMath::OffsetToCube(B);
	const int32 Distance = FMath::Max3(FMath::Abs(CubeA.X - CubeB.X), FMath::Abs(CubeA.Y - CubeB.Y), FMath::Abs(CubeA.Z - CubeB.Z));

	return Distance <= Range;
}

void ABattleGridManager::SetTargetIndicators(const TArray<FHexOffsetCoord>& Coords)
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->SetTargetIndicators(Coords);
}

void ABattleGridManager::ClearAllTargetIndicators()
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->ClearAllTargetIndicators();
}

const TArray<FBattleGridCell>& ABattleGridManager::GetGridCells(EBattleSimulationWorldType WorldType) const
{
	if (const FBattleGridState* State = GridStates.Find(WorldType)) return State->Cells;
	static const TArray<FBattleGridCell> EmptyCells;
	return EmptyCells;
}

TArray<FBattleGridCell>& ABattleGridManager::GetMutableGridCells(EBattleSimulationWorldType WorldType)
{
	return GridStates.FindOrAdd(WorldType).Cells;
}

bool ABattleGridManager::HasWorldState(EBattleSimulationWorldType WorldType) const
{
	const FBattleGridState* State = GridStates.Find(WorldType);
	return State && !State->Cells.IsEmpty();
}

bool ABattleGridManager::ResetSimulationWorldStateFromActual(EBattleSimulationWorldType WorldType, const TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& SimulationCharacterMap)
{
	if (WorldType == EBattleSimulationWorldType::PlayerActualEnemyActual || SimulationCharacterMap.IsEmpty()) return false;
	const FBattleGridState* ActualState = GridStates.Find(EBattleSimulationWorldType::PlayerActualEnemyActual);
	if (!ActualState || ActualState->Cells.IsEmpty()) return false;

	FBattleGridState RuntimeState;
	RuntimeState.Cells = ActualState->Cells;

	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
		ABattleSimulationCharacter* SimulationCharacter = Pair.Value.Get();
		if (!IsValid(SourceCharacter) || !IsValid(SimulationCharacter)) return false;
		bool bReplaced = false;
		for (FBattleGridCell& Cell : RuntimeState.Cells)
		{
			if (Cell.OccupyingActor != SourceCharacter) continue;
			Cell.OccupyingActor = SimulationCharacter;
			Cell.bOccupied = true;
			bReplaced = true;
			break;
		}
		if (!bReplaced) return false;
	}

	GridStates.Add(WorldType, MoveTemp(RuntimeState));
	return true;
}

void ABattleGridManager::RemoveWorldState(EBattleSimulationWorldType WorldType)
{
	if (WorldType == EBattleSimulationWorldType::PlayerActualEnemyActual) return;
	GridStates.Remove(WorldType);
}
