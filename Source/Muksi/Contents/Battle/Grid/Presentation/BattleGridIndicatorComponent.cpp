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

ABattleGridManager* UBattleGridIndicatorComponent::ResolveGridManager()
{
	if (!IsValid(GridManager))
	{
		GridManager = Cast<ABattleGridManager>(GetOwner());
	}
	return GridManager;
}

void UBattleGridIndicatorComponent::SetHovered(const TArray<FHexOffsetCoord>& Coords)
{
	if (!ResolveGridManager())
	{
		return;
	}

	ClearHovered();

	for (const FHexOffsetCoord& Coord : Coords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (!Tile)
		{
			continue;
		}

		HoveredCoords.AddUnique(Coord);
		Tile->SetTargetIndicatorVisible(true);
	}
}

void UBattleGridIndicatorComponent::ClearHovered()
{
	if (!ResolveGridManager())
	{
		return;
	}

	for (const FHexOffsetCoord& Coord : HoveredCoords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (Tile)
		{
			Tile->SetTargetIndicatorVisible(false);
		}
	}

	HoveredCoords.Empty();
}

void UBattleGridIndicatorComponent::ClearAllHovered()
{
	if (!ResolveGridManager())
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

	HoveredCoords.Empty();
}

void UBattleGridIndicatorComponent::SetExchange(const FBattleCardTypeInfoData& CardTypeInfo, const TArray<FHexOffsetCoord>& Coords, bool bEnemy)
{
	if (!ResolveGridManager())
	{
		return;
	}

	for (const FHexOffsetCoord& Coord : Coords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (Tile)
		{
			Tile->SetExchangeIndicator(CardTypeInfo, bEnemy);
		}
	}
}

void UBattleGridIndicatorComponent::ClearExchange()
{
	if (!ResolveGridManager())
	{
		return;
	}

	for (const FBattleGridCell& Cell : GridManager->GetGridCells(EBattleSimulationWorldType::PlayerActualEnemyActual))
	{
		if (Cell.TileActor)
		{
			Cell.TileActor->ClearExchangeIndicator();
		}
	}
}
