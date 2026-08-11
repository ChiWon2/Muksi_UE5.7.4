#include "Muksi/Contents/Battle/Grid/Generator/BattleGridTileGeneratorComponent.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"

UBattleGridTileGeneratorComponent::UBattleGridTileGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleGridTileGeneratorComponent::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

ABattleGridManager* UBattleGridTileGeneratorComponent::ResolveGridManager()
{
	if (!IsValid(GridManager))
	{
		GridManager = Cast<ABattleGridManager>(GetOwner());
	}

	return GridManager;
}

bool UBattleGridTileGeneratorComponent::GenerateTiles()
{
	if (!ResolveGridManager())
	{
		return false;
	}

	GenerateTilesInternal();
	return !GridManager->GridCells.IsEmpty();
}

void UBattleGridTileGeneratorComponent::ClearTiles()
{
	if (ResolveGridManager())
	{
		ClearTilesInternal();
	}
}

ABattleGridTile* UBattleGridTileGeneratorComponent::FindTile(const FHexOffsetCoord& Coord) const
{
	return IsValid(GridManager) ? GridManager->GetTileActorByCoord(Coord) : nullptr;
}

void UBattleGridTileGeneratorComponent::GenerateTilesInternal()
{
	ClearTilesInternal();

	const int32 ExpectedTileCount = LayoutSettings.GridWidth * LayoutSettings.GridHeight;

	if (TileClasses.Num() != ExpectedTileCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBattleGridTileGeneratorComponent::GenerateTiles - TileClasses count mismatch. Current: %d, Expected: %d"), TileClasses.Num(), ExpectedTileCount);
		return;
	}

	if (!GridManager->GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("UBattleGridTileGeneratorComponent::GenerateTiles - World is null"));
		return;
	}

	for (int32 Index = 0; Index < TileClasses.Num(); ++Index)
	{
		const TSubclassOf<ABattleGridTile> TileClass = TileClasses[Index];

		if (!TileClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("UBattleGridTileGeneratorComponent::GenerateTiles - TileClasses[%d] is null"), Index);
			continue;
		}

		const int32 X = Index % LayoutSettings.GridWidth;
		const int32 Y = Index / LayoutSettings.GridWidth;
		const FHexOffsetCoord Coord(X, Y);
		const FVector WorldLocation = GridManager->GetWorldLocationByCoord(Coord);
		const FRotator SpawnRotation = GridManager->GetActorRotation() + LayoutSettings.TileRotation;

		FBattleGridCell NewCell;
		NewCell.GridCoord = Coord;
		NewCell.WorldLocation = WorldLocation;
		NewCell.bWalkable = true;
		NewCell.bOccupied = false;
		NewCell.OccupyingActor = nullptr;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GridManager;

		ABattleGridTile* SpawnedTile = GridManager->GetWorld()->SpawnActor<ABattleGridTile>(
			TileClass, WorldLocation, SpawnRotation, SpawnParams);

		if (SpawnedTile)       
		{
			SpawnedTile->SetGridCoord(Coord);
			SpawnedTile->SetGridManager(GridManager);
			NewCell.WorldLocation = SpawnedTile->GetGridCenterWorldLocation();
			NewCell.TileActor = SpawnedTile;
		}

		GridManager->GridCells.Add(NewCell);
	}

	UE_LOG(LogTemp, Log, TEXT("Battle Grid generated: %d x %d"), LayoutSettings.GridWidth, LayoutSettings.GridHeight);
}

void UBattleGridTileGeneratorComponent::ClearTilesInternal()
{
	if (!IsValid(GridManager))
	{
		return;
	}

	for (FBattleGridCell& Cell : GridManager->GridCells)
	{
		if (IsValid(Cell.TileActor))
		{
			Cell.TileActor->Destroy();
		}
	}

	GridManager->GridCells.Empty();
	GridManager->TargetGridArray.Empty();
}
