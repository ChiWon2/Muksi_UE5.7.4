// Fill out your copyright notice in the Description page of Project Settings.

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"

// Sets default values
ABattleGridManager::ABattleGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BattleGridNavigationComponent = CreateDefaultSubobject<UBattleGridNavigationComponent>(TEXT("BattleGridNavigationComponent"));
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

FCubeCoord ABattleGridManager::OddQToCube(const FIntPoint& Coord) const
{
	const int32 Col = Coord.X;
	const int32 Row = Coord.Y;
	const int32 CubeX = Col;
	const int32 CubeZ = Row - (Col - (Col & 1)) / 2;
	const int32 CubeY = -CubeX - CubeZ;

	return FCubeCoord(CubeX, CubeY, CubeZ);
}

FIntPoint ABattleGridManager::CubeToOddQ(const FCubeCoord& Cube) const
{
	const int32 Col = Cube.X;
	const int32 Row = Cube.Z + (Cube.X - (Cube.X & 1)) / 2;

	return FIntPoint(Col, Row);
}

FCubeCoord ABattleGridManager::GetCubeDirection(int32 Direction) const
{
	Direction = ((Direction % 6) + 6) % 6;

	switch (Direction)
	{
	case 0:
		// 오른쪽
		return FCubeCoord(1, -1, 0);

	case 1:
		// 오른쪽 아래
		return FCubeCoord(1, 0, -1);

	case 2:
		// 왼쪽 아래
		return FCubeCoord(0, 1, -1);

	case 3:
		// 왼쪽
		return FCubeCoord(-1, 1, 0);

	case 4:
		// 왼쪽 위
		return FCubeCoord(-1, 0, 1);

	case 5:
		// 오른쪽 위
		return FCubeCoord(0, -1, 1);

	default:
		return FCubeCoord(1, -1, 0);
	}
}

FCubeCoord ABattleGridManager::RotateCubeRight60(const FCubeCoord& Cube) const
{
	// 시계 방향 60도
	return FCubeCoord(-Cube.Z, -Cube.X, -Cube.Y);
}

FCubeCoord ABattleGridManager::RotateCubeLeft60(const FCubeCoord& Cube) const
{
	// 반시계 방향 60도
	return FCubeCoord(-Cube.Y, -Cube.Z, -Cube.X);
}

bool ABattleGridManager::IsValidGridCoord(const FIntPoint& Coord) const
{
	return Coord.X >= 0 && Coord.X < GridWidth && Coord.Y >= 0 && Coord.Y < GridHeight;
}

ABattleGridTile* ABattleGridManager::GetTileByCoord(const FIntPoint& Coord) const
{
	if (!IsValidGridCoord(Coord))
	{
		return nullptr;
	}

	const int32 Index = CoordToIndex(Coord);

	if (!GridCells.IsValidIndex(Index))
	{
		return nullptr;
	}

	return GridCells[Index].TileActor;
}

//Test Hex Cell Dir Cal

void ABattleGridManager::MoveCharacter(ABattleCharacterBase* CharacterBase, FIntPoint InPoint)
{
	if (!CharacterBase)
	{
		return;
	}

	const FBattleGridCell* Cell = GetCell(InPoint);

	if (!Cell || !Cell->TileActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Move Character Index = %d"), CoordToIndex(InPoint));

	CharacterBase->SetActorTransform(Cell->TileActor->GetCharacterSpawnTransform());
	CharacterBase->SetCharacterPosition(InPoint);
}

void ABattleGridManager::GenerateGrid()
{
	ClearGrid();

	const int32 ExpectedTileCount = GridWidth * GridHeight;

	if (TileClasses.Num() != ExpectedTileCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABattleGridManager::GenerateGrid - TileClasses count mismatch. Current: %d, Expected: %d"), TileClasses.Num(), ExpectedTileCount);
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABattleGridManager::GenerateGrid - World is null"));
		return;
	}

	for (int32 Index = 0; Index < TileClasses.Num(); ++Index)
	{
		TSubclassOf<ABattleGridTile> TileClass = TileClasses[Index];

		if (!TileClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("ABattleGridManager::GenerateGrid - TileClasses[%d] is null"), Index);
			continue;
		}

		const int32 X = Index % GridWidth;
		const int32 Y = Index / GridWidth;
		const FIntPoint Coord(X, Y);
		const FVector WorldLocation = HexGridToWorld(Coord);
		const FRotator SpawnRotation = GetActorRotation() + TileRotation;

		FBattleGridCell NewCell;
		NewCell.GridCoord = Coord;
		NewCell.WorldLocation = WorldLocation;
		NewCell.bWalkable = true;
		NewCell.bOccupied = false;
		NewCell.OccupyingActor = nullptr;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ABattleGridTile* SpawnedTile = GetWorld()->SpawnActor<ABattleGridTile>(TileClass, WorldLocation, SpawnRotation, SpawnParams);

		if (SpawnedTile)
		{
			SpawnedTile->SetGridCoord(Coord);
			SpawnedTile->SetGridManager(this);
			NewCell.WorldLocation = SpawnedTile->GetGridCenterWorldLocation();
			NewCell.TileActor = SpawnedTile;
		}

		GridCells.Add(NewCell);
	}

	UE_LOG(LogTemp, Log, TEXT("BattleGridManager - Grid Generated From TileClasses: %d x %d"), GridWidth, GridHeight);
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
	TargetGridArray.Empty();
}

bool ABattleGridManager::IsValidCoord(const FIntPoint& Coord) const
{
	return Coord.X >= 0 && Coord.Y >= 0 && Coord.X < GridWidth && Coord.Y < GridHeight;
}

int32 ABattleGridManager::CoordToIndex(const FIntPoint& Coord) const
{
	return Coord.Y * GridWidth + Coord.X;
}

FVector ABattleGridManager::HexGridToWorld(const FIntPoint& Coord) const
{
	// Flat Top Hex + Odd-Q Offset 방식
	// X가 홀수인 열은 Y 방향으로 반 칸 내려감.

	const float LocalX = GridSpacingX * Coord.X;
	const float LocalY = GridSpacingY * (Coord.Y + OddColumnYOffsetRatio * (Coord.X & 1));
	const FVector LocalLocation(LocalX, LocalY, 0.0f);

	return GetActorTransform().TransformPosition(LocalLocation);
}

float ABattleGridManager::GetAdjacentTileCenterDistance() const
{
	float MinimumDistance = TNumericLimits<float>::Max();

	for (const FBattleGridCell& Cell : GridCells)
	{
		if (!Cell.TileActor)
		{
			continue;
		}

		const FVector CellLocation = Cell.TileActor->GetGridCenterWorldLocation();
		const TArray<FIntPoint> NeighborCoords = GetHexNeighbors(Cell.GridCoord);

		for (const FIntPoint& NeighborCoord : NeighborCoords)
		{
			const FBattleGridCell* NeighborCell = GetCell(NeighborCoord);

			if (!NeighborCell || !NeighborCell->TileActor)
			{
				continue;
			}

			const FVector NeighborLocation = NeighborCell->TileActor->GetGridCenterWorldLocation();
			const float Distance = FVector::Dist2D(CellLocation, NeighborLocation);

			if (Distance <= KINDA_SMALL_NUMBER)
			{
				continue;
			}

			MinimumDistance = FMath::Min(MinimumDistance, Distance);
		}
	}

	if (MinimumDistance != TNumericLimits<float>::Max())
	{
		return MinimumDistance;
	}

	const float DiagonalDistance = FMath::Sqrt(FMath::Square(GridSpacingX) + FMath::Square(GridSpacingY * OddColumnYOffsetRatio));

	return FMath::Min(GridSpacingY, DiagonalDistance);
}

float ABattleGridManager::GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius) const
{
	const int32 SafeGridRange = FMath::Max(0, GridRange);
	float WorldRadius = GetAdjacentTileCenterDistance() * SafeGridRange;

	if (bIncludeOuterTileRadius)
	{
		WorldRadius += HexRadius;
	}

	return WorldRadius;
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
		FIntPoint(+1, 0),
		FIntPoint(0, +1),
		FIntPoint(-1, 0),
		FIntPoint(-1, -1),
		FIntPoint(0, -1)
	};

	const TArray<FIntPoint> OddColumnDirections =
	{
		FIntPoint(+1, 0),
		FIntPoint(+1, +1),
		FIntPoint(0, +1),
		FIntPoint(-1, +1),
		FIntPoint(-1, 0),
		FIntPoint(0, -1)
	};

	const TArray<FIntPoint>& Directions = bIsOddColumn ? OddColumnDirections : EvenColumnDirections;

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

	const FTransform TargetTransform = GetTransformToPosition(ToCoord);

	Actor->SetActorLocation(TargetTransform.GetLocation());

	return true;
}

FTransform ABattleGridManager::GetTransformToPosition(FIntPoint InPosition)
{
	const FBattleGridCell* Cell = GetCell(InPosition);

	if (!Cell || !Cell->TileActor)
	{
		return FTransform::Identity;
	}

	return Cell->TileActor->GetCharacterSpawnTransform();
}

bool ABattleGridManager::CheckGridInRange(FIntPoint A, FIntPoint B, int32 Range)
{
	if (!IsValidCoord(A) || !IsValidCoord(B) || Range < 0)
	{
		return false;
	}

	const FCubeCoord CubeA = OddQToCube(A);
	const FCubeCoord CubeB = OddQToCube(B);
	const int32 Distance = FMath::Max3(FMath::Abs(CubeA.X - CubeB.X), FMath::Abs(CubeA.Y - CubeB.Y), FMath::Abs(CubeA.Z - CubeB.Z));

	return Distance <= Range;
}

void ABattleGridManager::SetGridHovered(TArray<FIntPoint> NewGridArray)
{
	TargetGridArray.Empty();

	for (const FIntPoint& Coord : NewGridArray)
	{
		ABattleGridTile* TargetGrid = GetTileByCoord(Coord);

		if (!TargetGrid)
		{
			continue;
		}

		TargetGridArray.AddUnique(Coord);
		TargetGrid->SetTargetIndicatorVisible(true);
	}
}

void ABattleGridManager::ClearGridHovered()
{
	for (const FIntPoint& Coord : TargetGridArray)
	{
		ABattleGridTile* TargetGrid = GetTileByCoord(Coord);

		if (!TargetGrid)
		{
			continue;
		}

		TargetGrid->SetTargetIndicatorVisible(false);
	}

	TargetGridArray.Empty();
}

void ABattleGridManager::AllClearGridHovered()
{
	for (FBattleGridCell& Cell : GridCells)
	{
		if (Cell.TileActor)
		{
			Cell.TileActor->SetTargetIndicatorVisible(false);
		}
	}

	TargetGridArray.Empty();
}

void ABattleGridManager::SetExchangeIndicator(int32 AttackType, TArray<FIntPoint> GridArray)
{
	for (const FIntPoint& Coord : GridArray)
	{
		ABattleGridTile* GridTile = GetTileByCoord(Coord);

		if (!GridTile)
		{
			continue;
		}

		GridTile->SetExchangeIndicator(AttackType);
	}
}

void ABattleGridManager::AllClearExchangeIndicator()
{
	for (FBattleGridCell& Cell : GridCells)
	{
		if (!Cell.TileActor)
		{
			continue;
		}

		Cell.TileActor->ClearExchangeIndicator();
	}
}

void ABattleGridManager::RushPosition(ABattleCharacterBase* BattleCharacter, FIntPoint TargetPoint)
{
	//일단 Move와 똑같이
	if (ABattleCharacter_Player* PlayerCharacter = Cast<ABattleCharacter_Player>(BattleCharacter))
	{
		MoveActorOnGrid(PlayerCharacter, PlayerCharacter->GetCharacterPosition(), TargetPoint);
	}
	else if (ABattleCharacter_Enemy* EnemyCharacter = Cast<ABattleCharacter_Enemy>(BattleCharacter))
	{
		MoveActorOnGrid(EnemyCharacter, EnemyCharacter->GetCharacterPosition(), TargetPoint);
	}
}

void ABattleGridManager::MovePosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint)
{
}

void ABattleGridManager::RangeAttackPosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint)
{
}