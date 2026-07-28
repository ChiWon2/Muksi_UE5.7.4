#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h" 

ABattleGridManager::ABattleGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	BattleGridNavigationComponent = CreateDefaultSubobject<UBattleGridNavigationComponent>(TEXT("BattleGridNavigationComponent"));
}

void ABattleGridManager::BeginPlay()
{
	Super::BeginPlay();

	if (bRuntimeClone)
	{
		return;
	}

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleGridManager>(this);
	}
}

void ABattleGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!bRuntimeClone)
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
		{
			ManagerSubsystem->UnregisterManager<ABattleGridManager>(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ABattleGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!bRuntimeClone)
	{
		GenerateGrid();
	}
}

FHexCubeCoord ABattleGridManager::OffsetToCube(const FHexOffsetCoord& Coord) const
{
	return FHexGridMath::OffsetToCube(Coord);
}

FHexOffsetCoord ABattleGridManager::CubeToOffset(const FHexCubeCoord& Cube) const
{
	return FHexGridMath::CubeToOffset(Cube);
}

FHexCubeCoord ABattleGridManager::GetCubeDirection(int32 Direction) const
{
	return FHexGridMath::GetCubeDirection(Direction);
}

FHexCubeCoord ABattleGridManager::RotateCubeRight60(const FHexCubeCoord& Cube) const
{
	// 시계 방향 60도
	return FHexCubeCoord(-Cube.Z, -Cube.X, -Cube.Y);
}

FHexCubeCoord ABattleGridManager::RotateCubeLeft60(const FHexCubeCoord& Cube) const
{
	// 반시계 방향 60도
	return FHexCubeCoord(-Cube.Y, -Cube.Z, -Cube.X);
}

bool ABattleGridManager::IsValidCoord(const FHexOffsetCoord& Coord) const
{
	return Coord.X >= 0 && Coord.X < GridWidth && Coord.Y >= 0 && Coord.Y < GridHeight;
}

ABattleGridTile* ABattleGridManager::GetTileByCoord(const FHexOffsetCoord& Coord) const
{
	const FBattleGridCell* Cell = GetCell(Coord);
	return Cell ? Cell->TileActor.Get() : nullptr;
}

//Test Hex Cell Dir Cal
void ABattleGridManager::MoveCharacter(ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& InPoint)
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
		const FHexOffsetCoord Coord(X, Y);
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
	if (!bRuntimeClone)
	{
		for (FBattleGridCell& Cell : GridCells)
		{
			if (IsValid(Cell.TileActor))
			{
				Cell.TileActor->Destroy();
			}
		}
	}

	GridCells.Empty();
	TargetGridArray.Empty();
}

int32 ABattleGridManager::CoordToIndex(const FHexOffsetCoord& Coord) const
{
	return Coord.Y * GridWidth + Coord.X;
}

FVector ABattleGridManager::HexGridToWorld(const FHexOffsetCoord& Coord) const
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
		const TArray<FHexOffsetCoord> NeighborCoords = GetHexNeighbors(Cell.GridCoord);

		for (const FHexOffsetCoord& NeighborCoord : NeighborCoords)
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

FBattleGridCell* ABattleGridManager::GetCell(const FHexOffsetCoord& Coord)
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

const FBattleGridCell* ABattleGridManager::GetCell(const FHexOffsetCoord& Coord) const
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

TArray<FHexOffsetCoord> ABattleGridManager::GetHexNeighbors(const FHexOffsetCoord& Coord) const
{
	TArray<FHexOffsetCoord> Neighbors;

	if (!IsValidCoord(Coord))
	{
		return Neighbors;
	}

	const bool bIsOddColumn = (Coord.X & 1) == 1;

	const TArray<FHexOffsetCoord> EvenColumnDirections =
	{
		FHexOffsetCoord(+1, -1),
		FHexOffsetCoord(+1, 0),
		FHexOffsetCoord(0, +1),
		FHexOffsetCoord(-1, 0),
		FHexOffsetCoord(-1, -1),
		FHexOffsetCoord(0, -1)
	};

	const TArray<FHexOffsetCoord> OddColumnDirections =
	{
		FHexOffsetCoord(+1, 0),
		FHexOffsetCoord(+1, +1),
		FHexOffsetCoord(0, +1),
		FHexOffsetCoord(-1, +1),
		FHexOffsetCoord(-1, 0),
		FHexOffsetCoord(0, -1)
	};

	const TArray<FHexOffsetCoord>& Directions = bIsOddColumn ? OddColumnDirections : EvenColumnDirections;

	for (const FHexOffsetCoord& Direction : Directions)
	{
		const FHexOffsetCoord NextCoord = Coord + Direction;

		if (IsValidCoord(NextCoord))
		{
			Neighbors.Add(NextCoord);
		}
	}

	return Neighbors;
}

TArray<FHexOffsetCoord> ABattleGridManager::GetMovableCoords(const FHexOffsetCoord& StartCoord, int32 MoveRange) const
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
			Queue.Enqueue(TPair<FHexOffsetCoord, int32>(NextCoord, CurrentDistance + 1));
		}
	}

	return Result;
}

bool ABattleGridManager::SetOccupied(const FHexOffsetCoord& Coord, AActor* Actor)
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

bool ABattleGridManager::ClearOccupied(const FHexOffsetCoord& Coord)
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

bool ABattleGridManager::MoveActorOnGrid(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord)
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

FTransform ABattleGridManager::GetTransformToPosition(const FHexOffsetCoord& InPosition)
{
	const FBattleGridCell* Cell = GetCell(InPosition);

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

	const FHexCubeCoord CubeA = OffsetToCube(A);
	const FHexCubeCoord CubeB = OffsetToCube(B);
	const int32 Distance = FMath::Max3(FMath::Abs(CubeA.X - CubeB.X), FMath::Abs(CubeA.Y - CubeB.Y), FMath::Abs(CubeA.Z - CubeB.Z));

	return Distance <= Range;
}

void ABattleGridManager::SetGridHovered(const TArray<FHexOffsetCoord>& NewGridArray)
{
	TargetGridArray.Empty();

	for (const FHexOffsetCoord& Coord : NewGridArray)
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
	for (const FHexOffsetCoord& Coord : TargetGridArray)
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

void ABattleGridManager::SetExchangeIndicator(int32 AttackType, const TArray<FHexOffsetCoord>& GridArray)
{
	for (const FHexOffsetCoord& Coord : GridArray)
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

bool ABattleGridManager::InitializeRuntimeClone(const ABattleGridManager* SourceGridManager)
{
	if (!IsValid(SourceGridManager))
	{
		return false;
	}

	bRuntimeClone = true;
	GridWidth = SourceGridManager->GridWidth;
	GridHeight = SourceGridManager->GridHeight;
	HexRadius = SourceGridManager->HexRadius;
	GridSpacingX = SourceGridManager->GridSpacingX;
	GridSpacingY = SourceGridManager->GridSpacingY;
	OddColumnYOffsetRatio = SourceGridManager->OddColumnYOffsetRatio;
	TileRotation = SourceGridManager->TileRotation;
	TileClasses = SourceGridManager->TileClasses;
	GridCells = SourceGridManager->GridCells;
	TargetGridArray.Empty();

	return !GridCells.IsEmpty();
}

bool ABattleGridManager::ReplaceOccupyingActor(AActor* SourceActor, AActor* ReplacementActor)
{
	if (!IsValid(SourceActor) || !IsValid(ReplacementActor))
	{
		return false;
	}

	for (FBattleGridCell& Cell : GridCells)
	{
		if (Cell.OccupyingActor == SourceActor)
		{
			Cell.OccupyingActor = ReplacementActor;
			return true;
		}
	}

	return false;
}
