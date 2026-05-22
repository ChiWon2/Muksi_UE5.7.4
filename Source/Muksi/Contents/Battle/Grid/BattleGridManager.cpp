// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"

// Sets default values
ABattleGridManager::ABattleGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
}

// Called when the game starts or when spawned
void ABattleGridManager::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ABattleGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateGrid();
}

// Called every frame
void ABattleGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ABattleGridManager::GenerateGrid()
{
	ClearGrid();
	
	const int32 ExpectedTileCount = GridWidth * GridHeight;
	
	if (TileClasses.Num() != ExpectedTileCount)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ABattleGridManager::GenerateGrid - TileClasses count mismatch. Current: %d, Expected: %d"),
			TileClasses.Num(),
			ExpectedTileCount
		);
		return;
	}
	
	for (int32 Index = 0; Index < TileClasses.Num(); ++Index)
	{
		TSubclassOf<ABattleGridTile> TileClass = TileClasses[Index];

		if (!TileClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("ABattleGridManager::GenerateGrid - TileClasses[%d] is null"),
				Index
			);
			continue;
		}

		const int32 X = Index % GridX;
		const int32 Y = Index / GridY;

		const FIntPoint Coord(X, Y);
		const FVector WorldLocation = HexGridToWorld(Coord);

		FBattleGridCell NewCell;
		NewCell.GridCoord = Coord;
		NewCell.WorldLocation = WorldLocation;
		NewCell.bWalkable = true;
		NewCell.bOccupied = false;
		NewCell.OccupyingActor = nullptr;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ABattleGridTile* SpawnedTile = GetWorld()->SpawnActor<ABattleGridTile>(
			TileClass,
			WorldLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (SpawnedTile)
		{
			SpawnedTile->SetGridCoord(Coord);
			SpawnedTile->SetGridManager(this);

			NewCell.TileActor = SpawnedTile;
		}

		GridCells.Add(NewCell);
	}

	UE_LOG(LogTemp, Log,
		TEXT("BattleGridManager - Grid Generated From TileClasses: %d x %d"),
		GridWidth,
		GridHeight
	);
	
	//TSubclass create function
	/*if (!TileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABattleGridManager::GenerateGrid - TileClass is null"));
		return;
	}*/
	
	
	
	/*for (int32 Y = 0; Y < GridHeight; ++Y)
	{
		for (int32 X = 0; X < GridWidth; ++X)
		{
			const FIntPoint Coord(X, Y);
			const FVector WorldLocation = HexGridToWorld(Coord);

			FBattleGridCell NewCell;
			NewCell.GridCoord = Coord;
			NewCell.WorldLocation = WorldLocation;
			NewCell.bWalkable = true;
			NewCell.bOccupied = false;
			NewCell.OccupyingActor = nullptr;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			ABattleGridTile* SpawnedTile = GetWorld()->SpawnActor<ABattleGridTile>(
				TileClass,
				WorldLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (SpawnedTile)
			{
				SpawnedTile->SetGridCoord(Coord);
				SpawnedTile->SetGridManager(this);

				NewCell.TileActor = SpawnedTile;
			}

			GridCells.Add(NewCell);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("BattleGridManager - Grid Generated: %d x %d"), GridWidth, GridHeight);*/
}

void ABattleGridManager::ClearGrid()
{
	for (FBattleGridCell& Cell : GridCells)
	{
		if (IsValid(Cell.TileActor))
		{
			Cell.TileActor->Destroy();
		}
	}

	GridCells.Empty();
}

bool ABattleGridManager::IsValidCoord(const FIntPoint& Coord) const
{
	return Coord.X >= 0 &&
		Coord.Y >= 0 &&
		Coord.X < GridWidth &&
		Coord.Y < GridHeight;
}

int32 ABattleGridManager::CoordToIndex(const FIntPoint& Coord) const
{
	return Coord.Y * GridWidth + Coord.X;
}

FVector ABattleGridManager::HexGridToWorld(const FIntPoint& Coord) const
{
	// Flat Top Hex + Odd-Q Offset 방식
	// X가 홀수인 열은 Y 방향으로 반 칸 내려감.

	const float WorldX = HexRadius * 1.5f * Coord.X;
	const float WorldY = HexRadius * FMath::Sqrt(3.0f) * (Coord.Y + 0.5f * (Coord.X & 1));

	return GetActorLocation() + FVector(WorldX, WorldY, 0.0f);
}

FBattleGridCell* ABattleGridManager::GetCell(const FIntPoint& Coord)
{
	if (!IsValidCoord(Coord))
	{
		return nullptr;
	}

	const int32 Index = CoordToIndex(Coord);

	if (!GridCells.IsValidIndex(Index))
	{
		return nullptr;
	}

	return &GridCells[Index];
}

const FBattleGridCell* ABattleGridManager::GetCell(const FIntPoint& Coord) const
{
	if (!IsValidCoord(Coord))
	{
		return nullptr;
	}

	const int32 Index = CoordToIndex(Coord);

	if (!GridCells.IsValidIndex(Index))
	{
		return nullptr;
	}

	return &GridCells[Index];
}

TArray<FIntPoint> ABattleGridManager::GetHexNeighbors(const FIntPoint& Coord) const
{
	TArray<FIntPoint> Neighbors;

	if (!IsValidCoord(Coord))
	{
		return Neighbors;
	}

	const bool bIsOddColumn = (Coord.X & 1) == 1;

	const TArray<FIntPoint> EvenColumnDirections =
	{
		FIntPoint(+1, -1),
		FIntPoint(+1,  0),
		FIntPoint( 0, +1),
		FIntPoint(-1,  0),
		FIntPoint(-1, -1),
		FIntPoint( 0, -1)
	};

	const TArray<FIntPoint> OddColumnDirections =
	{
		FIntPoint(+1,  0),
		FIntPoint(+1, +1),
		FIntPoint( 0, +1),
		FIntPoint(-1, +1),
		FIntPoint(-1,  0),
		FIntPoint( 0, -1)
	};

	const TArray<FIntPoint>& Directions = bIsOddColumn
		? OddColumnDirections
		: EvenColumnDirections;

	for (const FIntPoint& Direction : Directions)
	{
		const FIntPoint NextCoord = Coord + Direction;

		if (IsValidCoord(NextCoord))
		{
			Neighbors.Add(NextCoord);
		}
	}

	return Neighbors;
}

TArray<FIntPoint> ABattleGridManager::GetMovableCoords(const FIntPoint& StartCoord, int32 MoveRange) const
{
	TArray<FIntPoint> Result;

	if (!IsValidCoord(StartCoord))
	{
		return Result;
	}

	if (MoveRange <= 0)
	{
		return Result;
	}

	TQueue<TPair<FIntPoint, int32>> Queue;
	TSet<FIntPoint> Visited;

	Queue.Enqueue(TPair<FIntPoint, int32>(StartCoord, 0));
	Visited.Add(StartCoord);

	while (!Queue.IsEmpty())
	{
		TPair<FIntPoint, int32> Current;
		Queue.Dequeue(Current);

		const FIntPoint CurrentCoord = Current.Key;
		const int32 CurrentDistance = Current.Value;

		if (CurrentDistance > 0)
		{
			Result.Add(CurrentCoord);
		}

		if (CurrentDistance >= MoveRange)
		{
			continue;
		}

		const TArray<FIntPoint> Neighbors = GetHexNeighbors(CurrentCoord);

		for (const FIntPoint& NextCoord : Neighbors)
		{
			if (Visited.Contains(NextCoord))
			{
				continue;
			}

			const FBattleGridCell* NextCell = GetCell(NextCoord);
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
			Queue.Enqueue(TPair<FIntPoint, int32>(NextCoord, CurrentDistance + 1));
		}
	}

	return Result;
}

bool ABattleGridManager::SetOccupied(const FIntPoint& Coord, AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	FBattleGridCell* Cell = GetCell(Coord);
	if (!Cell)
	{
		return false;
	}

	if (!Cell->bWalkable)
	{
		return false;
	}

	if (Cell->bOccupied)
	{
		return false;
	}

	Cell->bOccupied = true;
	Cell->OccupyingActor = Actor;

	return true;
}

bool ABattleGridManager::ClearOccupied(const FIntPoint& Coord)
{
	FBattleGridCell* Cell = GetCell(Coord);
	if (!Cell)
	{
		return false;
	}

	Cell->bOccupied = false;
	Cell->OccupyingActor = nullptr;

	return true;
}

bool ABattleGridManager::MoveActorOnGrid(AActor* Actor, const FIntPoint& FromCoord, const FIntPoint& ToCoord)
{
	if (!Actor)
	{
		return false;
	}

	FBattleGridCell* FromCell = GetCell(FromCoord);
	FBattleGridCell* ToCell = GetCell(ToCoord);

	if (!FromCell || !ToCell)
	{
		return false;
	}

	if (!ToCell->bWalkable)
	{
		return false;
	}

	if (ToCell->bOccupied)
	{
		return false;
	}

	FromCell->bOccupied = false;
	FromCell->OccupyingActor = nullptr;

	ToCell->bOccupied = true;
	ToCell->OccupyingActor = Actor;

	Actor->SetActorLocation(ToCell->WorldLocation);

	return true;
}

