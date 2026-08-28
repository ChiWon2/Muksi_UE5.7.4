#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"

UBattleGridIndicatorComponent::UBattleGridIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleGridIndicatorComponent::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

ABattleGridManager* UBattleGridIndicatorComponent::GetGridManager()
{
	if (!IsValid(GridManager))
	{
		GridManager = Cast<ABattleGridManager>(GetOwner());
	}
	return GridManager;
}

void UBattleGridIndicatorComponent::SetTargetIndicators(const TArray<FHexOffsetCoord>& Coords)
{
	if (!GetGridManager())
	{
		return;
	}

	ClearTargetIndicators();

	for (const FHexOffsetCoord& Coord : Coords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (!Tile)
		{
			continue;
		}

		TargetIndicatorCoords.AddUnique(Coord);
		Tile->SetTargetIndicatorVisible(true);
	}
}

void UBattleGridIndicatorComponent::ClearTargetIndicators()
{
	if (!GetGridManager())
	{
		return;
	}

	for (const FHexOffsetCoord& Coord : TargetIndicatorCoords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (Tile)
		{
			Tile->SetTargetIndicatorVisible(false);
		}
	}

	TargetIndicatorCoords.Empty();
}

void UBattleGridIndicatorComponent::ClearAllTargetIndicators()
{
	if (!GetGridManager())
	{
		return;
	}

	for (const FBattleGridCell& Cell : GridManager->GetGridCells(EBattleSimulationWorldType::PlayerActualEnemyActual))
	{
		if (Cell.TileActor)
		{
			Cell.TileActor->SetTargetIndicatorVisible(false);
		}
	}

	TargetIndicatorCoords.Empty();
}
