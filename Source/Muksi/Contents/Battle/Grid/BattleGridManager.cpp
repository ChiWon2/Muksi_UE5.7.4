#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Grid/Generator/BattleGridTileGeneratorComponent.h"
#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h" 

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


	if (bWorldManagerRegistrationEnabled)
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
		{
			ManagerSubsystem->RegisterManager<ABattleGridManager>(this);
		}
	}
}

void ABattleGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bWorldManagerRegistrationEnabled)
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
		{
			ManagerSubsystem->UnregisterManager<ABattleGridManager>(this);
		}
	}
	RuntimeSourceGridManager = nullptr;
	bRuntimeGridInstance = false;
	Super::EndPlay(EndPlayReason);
}

void ABattleGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bGenerateGridOnConstruction)
	{
		GenerateGrid();
	}
}

bool ABattleGridManager::IsValidCoord(const FHexOffsetCoord& Coord) const
{
	const FBattleGridLayoutSettings& Layout = GetLayoutSettings();
	return Coord.X >= 0 && Coord.X < Layout.GridWidth && Coord.Y >= 0 && Coord.Y < Layout.GridHeight;
}

ABattleGridTile* ABattleGridManager::GetTileActorByCoord(const FHexOffsetCoord& Coord)
{
	FBattleGridCell* Cell = GetCellByCoord(Coord);
	return Cell ? Cell->TileActor.Get() : nullptr;
}

const FBattleGridLayoutSettings& ABattleGridManager::GetLayoutSettings() const
{
	if (IsValid(RuntimeSourceGridManager))
	{
		return RuntimeSourceGridManager->GetLayoutSettings();
	}
	check(BattleGridGeneratorComponent);
	return BattleGridGeneratorComponent->GetLayoutSettings();
}

//Test Hex Cell Dir Cal
void ABattleGridManager::PlaceCharacter(ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& Coord)
{
	if (!CharacterBase)
	{
		return;
	}

	const FBattleGridCell* Cell = GetCellByCoord(Coord);

	if (!Cell || !Cell->TileActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Move Character Index = %d"), CoordToIndex(Coord));

	CharacterBase->SetActorTransform(Cell->TileActor->GetCharacterSpawnTransform());
	CharacterBase->SetCharacterPosition(Coord);
	SetOccupied(Coord, CharacterBase);
}

void ABattleGridManager::GenerateGrid()
{
	if (IsRuntimeGridInstance())
	{
		return;
	}
	if (BattleGridGeneratorComponent)
	{
		BattleGridGeneratorComponent->GenerateTiles();
	}
}

void ABattleGridManager::ClearGrid()
{
	if (IsRuntimeGridInstance())
	{
		GridCells.Reset();
		TargetGridArray.Reset();
		return;
	}
	if (BattleGridGeneratorComponent)
	{
		BattleGridGeneratorComponent->ClearTiles();
	}
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

float ABattleGridManager::GetAdjacentTileCenterDistance()
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
			FBattleGridCell* NeighborCell = GetCellByCoord(NeighborCoord);

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

	const FBattleGridLayoutSettings& Layout = GetLayoutSettings();
	const float DiagonalDistance = FMath::Sqrt(
		FMath::Square(Layout.GridSpacingX * Layout.OddRowXOffsetRatio) +
		FMath::Square(Layout.GridSpacingY));

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

FBattleGridCell* ABattleGridManager::GetCellByCoord(const FHexOffsetCoord& Coord)
{
	if (!IsValidCoord(Coord))
	{
		return nullptr;
	}

	TArray<FBattleGridCell>& ActiveCells = GetActiveGridCells();
	const int32 Index = CoordToIndex(Coord);
	return ActiveCells.IsValidIndex(Index) ? &ActiveCells[Index] : nullptr;
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

TArray<FHexOffsetCoord> ABattleGridManager::GetMovableCoords(const FHexOffsetCoord& StartCoord, int32 MoveRange)
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

			FBattleGridCell* NextCell = GetCellByCoord(NextCoord);

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
	if (!IsValid(Actor))
	{
		return false;
	}

	FBattleGridCell* DestinationCell = GetCellByCoord(Coord);
	if (!DestinationCell || !DestinationCell->bWalkable || DestinationCell->bOccupied)
	{
		return false;
	}

	// 같은 Actor가 이미 다른 셀에 있다면 기존 점유를 해제한다.
	for (FBattleGridCell& Cell : GetActiveGridCells())
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

bool ABattleGridManager::ClearOccupied(const FHexOffsetCoord& Coord)
{
	FBattleGridCell* Cell = GetCellByCoord(Coord);
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

	FBattleGridCell* FromCell = GetCellByCoord(Request.FromCoord);
	FBattleGridCell* ToCell = GetCellByCoord(Request.ToCoord);
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

bool ABattleGridManager::MoveCharacterOnGrid(ABattleCharacterBase* Character, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord, bool bSnapActorToGrid)
{
	FBattleGridMoveRequest Request;
	Request.Character = Character;
	Request.FromCoord = FromCoord;
	Request.ToCoord = ToCoord;
	Request.bSnapActorToGrid = bSnapActorToGrid;
	return ExecuteGridMove(Request).bSucceeded;
}

bool ABattleGridManager::MoveActorOnGrid(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord)
{
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(Actor))
	{
		return MoveCharacterOnGrid(Character, FromCoord, ToCoord, true);
	}

	if (!IsValid(Actor) || FromCoord == ToCoord)
	{
		return false;
	}

	FBattleGridCell* FromCell = GetCellByCoord(FromCoord);
	FBattleGridCell* ToCell = GetCellByCoord(ToCoord);

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
	const FBattleGridCell* Cell = GetCellByCoord(InPosition);

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

void ABattleGridManager::SetGridHovered(const TArray<FHexOffsetCoord>& NewGridArray)
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->SetHovered(NewGridArray);
}

void ABattleGridManager::ClearGridHovered()
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->ClearHovered();
}

void ABattleGridManager::AllClearGridHovered()
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->ClearAllHovered();
}

void ABattleGridManager::SetExchangeIndicator(const EMuksiBattleCardType& BattleCardType, const TArray<FHexOffsetCoord>& GridArray, bool bEnemy)
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->SetExchange(BattleCardType, GridArray, bEnemy);
}

void ABattleGridManager::AllClearExchangeIndicator()
{
	if (BattleGridIndicatorComponent) BattleGridIndicatorComponent->ClearExchange();
}


TArray<FBattleGridCell>& ABattleGridManager::GetActiveGridCells()
{
	return GridCells;
}

bool ABattleGridManager::InitializeRuntimeGridFromSource(ABattleGridManager* InSourceGridManager)
{
	if (!IsValid(InSourceGridManager) || InSourceGridManager == this)
	{
		return false;
	}
	RuntimeSourceGridManager = InSourceGridManager;
	bRuntimeGridInstance = true;
	GridCells = InSourceGridManager->GridCells;
	TargetGridArray.Reset();
	SetActorTransform(InSourceGridManager->GetActorTransform());
	return !GridCells.IsEmpty();
}

bool ABattleGridManager::ReplaceGridActor(AActor* SourceActor, AActor* ReplacementActor)
{
	if (!IsRuntimeGridInstance() || !IsValid(SourceActor) || !IsValid(ReplacementActor))
	{
		return false;
	}
	for (FBattleGridCell& Cell : GridCells)
	{
		if (Cell.OccupyingActor != SourceActor)
		{
			continue;
		}
		Cell.OccupyingActor = ReplacementActor;
		Cell.bOccupied = true;
		return true;
	}
	return false;
}
