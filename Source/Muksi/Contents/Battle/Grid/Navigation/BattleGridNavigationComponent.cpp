#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"

#include "Algo/Reverse.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

UBattleGridNavigationComponent::UBattleGridNavigationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleGridNavigationComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheGridManager();
}

void UBattleGridNavigationComponent::CacheGridManager()
{
	if (GridManager)
	{
		return;
	}

	GridManager = Cast<ABattleGridManager>(GetOwner());

	if (!GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleGridNavigationComponent] Owner is not ABattleGridManager. Owner=%s"), *GetNameSafe(GetOwner()));
	}
}

bool UBattleGridNavigationComponent::IsCellAvailable(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord, const AActor* IgnoredActor) const
{
	if (!GridManager)
	{
		return false;
	}

	if (!GridManager->IsValidCoord(Coord))
	{
		return false;
	}

	const FBattleGridCell* Cell = GridManager->GetCellByCoord(WorldType, Coord);

	if (!Cell)
	{
		return false;
	}

	if (!Cell->bWalkable)
	{
		return false;
	}

	if (!Cell->bOccupied)
	{
		return true;
	}

	if (IgnoredActor && Cell->OccupyingActor == IgnoredActor)
	{
		return true;
	}

	return false;
}

float UBattleGridNavigationComponent::GetCellMovementCost(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord) const
{
	if (!GridManager)
	{
		return TNumericLimits<float>::Max();
	}

	const FBattleGridCell* Cell = GridManager->GetCellByCoord(WorldType, Coord);

	if (!Cell)
	{
		return TNumericLimits<float>::Max();
	}

	if (!Cell->bWalkable)
	{
		return TNumericLimits<float>::Max();
	}

	/**
	 * 현재는 모든 Cell이 같은 비용을 사용한다.
	 *
	 * 나중에 FBattleGridCell에 MovementCost가 추가되면:
	 *
	 * return FMath::Max(
	 *     MinimumMovementCost,
	 *     Cell->MovementCost
	 * );
	 */
	return FMath::Max(MinimumMovementCost, DefaultMovementCost);
}

float UBattleGridNavigationComponent::CalculateHexDistance(const FHexOffsetCoord& A, const FHexOffsetCoord& B) const
{
	if (!GridManager)
	{
		return 0.0f;
	}

	const FHexCubeCoord CubeA = FHexGridMath::OffsetToCube(A);
	const FHexCubeCoord CubeB = FHexGridMath::OffsetToCube(B);
	const int32 Distance = FMath::Max3(FMath::Abs(CubeA.X - CubeB.X), FMath::Abs(CubeA.Y - CubeB.Y), FMath::Abs(CubeA.Z - CubeB.Z));

	return static_cast<float>(Distance);
}

bool UBattleGridNavigationComponent::FindGroundPath(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, TArray<FHexOffsetCoord>& OutPath, const AActor* MovingActor) const
{
	OutPath.Empty();

	if (!GridManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGridNavigationComponent] FindGroundPath failed: GridManager is null."));
		return false;
	}

	if (!GridManager->IsValidCoord(StartCoord))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGridNavigationComponent] Invalid StartCoord: (%d, %d)"), StartCoord.X, StartCoord.Y);
		return false;
	}

	if (!GridManager->IsValidCoord(DestinationCoord))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGridNavigationComponent] Invalid DestinationCoord: (%d, %d)"), DestinationCoord.X, DestinationCoord.Y);
		return false;
	}

	if (StartCoord == DestinationCoord)
	{
		return true;
	}

	if (!IsCellAvailable(WorldType, DestinationCoord, MovingActor))
	{
		UE_LOG(LogTemp, Log, TEXT("[BattleGridNavigationComponent] Destination is not available: (%d, %d)"), DestinationCoord.X, DestinationCoord.Y);
		return false;
	}

	TArray<FHexOffsetCoord> OpenSet;
	TSet<FHexOffsetCoord> ClosedSet;
	TMap<FHexOffsetCoord, FHexOffsetCoord> CameFrom;
	TMap<FHexOffsetCoord, float> GScore;
	TMap<FHexOffsetCoord, float> FScore;

	OpenSet.Add(StartCoord);
	GScore.Add(StartCoord, 0.0f);

	const float StartHeuristic = CalculateHexDistance(StartCoord, DestinationCoord) * MinimumMovementCost;

	FScore.Add(StartCoord, StartHeuristic);

	while (!OpenSet.IsEmpty())
	{
		FHexOffsetCoord CurrentCoord;

		if (!FindLowestScoreCoord(OpenSet, FScore, CurrentCoord))
		{
			break;
		}

		if (CurrentCoord == DestinationCoord)
		{
			const bool bReconstructed = ReconstructPath(StartCoord, DestinationCoord, CameFrom, OutPath);

			if (bReconstructed)
			{
				UE_LOG(LogTemp, Log, TEXT("[BattleGridNavigationComponent] A* path found. Start=(%d, %d) Destination=(%d, %d) PathLength=%d"), StartCoord.X, StartCoord.Y, DestinationCoord.X, DestinationCoord.Y, OutPath.Num());
			}

			return bReconstructed;
		}

		OpenSet.Remove(CurrentCoord);
		ClosedSet.Add(CurrentCoord);

		const float CurrentGScore = GScore.Contains(CurrentCoord) ? GScore[CurrentCoord] : TNumericLimits<float>::Max();
		const TArray<FHexOffsetCoord> Neighbors = GridManager->GetHexNeighbors(CurrentCoord);

		for (const FHexOffsetCoord& NeighborCoord : Neighbors)
		{
			if (ClosedSet.Contains(NeighborCoord))
			{
				continue;
			}

			if (!IsCellAvailable(WorldType, NeighborCoord, MovingActor))
			{
				continue;
			}

			const float MovementCost = GetCellMovementCost(WorldType, NeighborCoord);

			if (MovementCost >= TNumericLimits<float>::Max())
			{
				continue;
			}

			const float TentativeGScore = CurrentGScore + MovementCost;
			const float ExistingGScore = GScore.Contains(NeighborCoord) ? GScore[NeighborCoord] : TNumericLimits<float>::Max();

			if (TentativeGScore >= ExistingGScore)
			{
				continue;
			}

			CameFrom.Add(NeighborCoord, CurrentCoord);
			GScore.Add(NeighborCoord, TentativeGScore);

			const float HeuristicScore = CalculateHexDistance(NeighborCoord, DestinationCoord) * MinimumMovementCost;

			FScore.Add(NeighborCoord, TentativeGScore + HeuristicScore);

			if (!OpenSet.Contains(NeighborCoord))
			{
				OpenSet.Add(NeighborCoord);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGridNavigationComponent] A* path not found. Start=(%d, %d) Destination=(%d, %d)"), StartCoord.X, StartCoord.Y, DestinationCoord.X, DestinationCoord.Y);

	return false;
}

bool UBattleGridNavigationComponent::FindLowestScoreCoord(const TArray<FHexOffsetCoord>& OpenSet, const TMap<FHexOffsetCoord, float>& FScore, FHexOffsetCoord& OutCoord) const
{
	if (OpenSet.IsEmpty())
	{
		return false;
	}

	bool bFound = false;
	float LowestScore = TNumericLimits<float>::Max();

	for (const FHexOffsetCoord& Coord : OpenSet)
	{
		const float Score = FScore.Contains(Coord) ? FScore[Coord] : TNumericLimits<float>::Max();

		if (!bFound || Score < LowestScore)
		{
			bFound = true;
			LowestScore = Score;
			OutCoord = Coord;
		}
	}

	return bFound;
}

bool UBattleGridNavigationComponent::ReconstructPath(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, const TMap<FHexOffsetCoord, FHexOffsetCoord>& CameFrom, TArray<FHexOffsetCoord>& OutPath) const
{
	OutPath.Empty();

	if (StartCoord == DestinationCoord)
	{
		return true;
	}

	FHexOffsetCoord CurrentCoord = DestinationCoord;

	while (CurrentCoord != StartCoord)
	{
		OutPath.Add(CurrentCoord);

		const FHexOffsetCoord* PreviousCoord = CameFrom.Find(CurrentCoord);

		if (!PreviousCoord)
		{
			UE_LOG(LogTemp, Error, TEXT("[BattleGridNavigationComponent] Failed to reconstruct path at (%d, %d)"), CurrentCoord.X, CurrentCoord.Y);
			OutPath.Empty();
			return false;
		}

		CurrentCoord = *PreviousCoord;
	}

	Algo::Reverse(OutPath);

	return true;
}

bool UBattleGridNavigationComponent::ConvertGridPathToWorldPath(const TArray<FHexOffsetCoord>& GridPath, TArray<FVector>& OutWorldPath) const
{
	OutWorldPath.Empty();

	if (!GridManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGridNavigationComponent] ConvertGridPathToWorldPath failed: GridManager is null."));
		return false;
	}

	/*
	 * 경로가 비어 있는 경우는
	 * 이미 목적지에 있는 것으로 처리할 수 있다.
	 *
	 * 변환 실패가 아니므로 true를 반환한다.
	 */
	if (GridPath.IsEmpty())
	{
		return true;
	}

	OutWorldPath.Reserve(GridPath.Num());

	for (const FHexOffsetCoord& GridCoord : GridPath)
	{
		FVector WorldLocation = FVector::ZeroVector;

		if (!GetGridWorldLocation(GridCoord, WorldLocation))
		{
			UE_LOG(LogTemp, Warning, TEXT("[BattleGridNavigationComponent] Failed to convert Grid Coord to World Location. Coord=(%d, %d)"), GridCoord.X, GridCoord.Y);
			OutWorldPath.Empty();
			return false;
		}

		OutWorldPath.Add(WorldLocation);
	}

	return true;
}

bool UBattleGridNavigationComponent::GetGridWorldLocation(const FHexOffsetCoord& Coord, FVector& OutWorldLocation) const
{
	OutWorldLocation = FVector::ZeroVector;

	if (!GridManager)
	{
		return false;
	}

	if (!GridManager->IsValidCoord(Coord))
	{
		return false;
	}

	const FBattleGridCell* Cell = GridManager->GetCellByCoord(EBattleSimulationWorldType::PlayerActualEnemyActual, Coord);

	if (!Cell)
	{
		return false;
	}

	if (!Cell->TileActor)
	{
		return false;
	}

	const FTransform TargetTransform = GridManager->GetTransformToPosition(Coord);

	OutWorldLocation = TargetTransform.GetLocation();

	return true;
}
