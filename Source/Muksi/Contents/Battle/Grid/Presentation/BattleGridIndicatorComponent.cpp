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

	if (!GridManager->IsTilePresentationEnabled())
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

		GridManager->TargetGridArray.AddUnique(Coord);
		Tile->SetTargetIndicatorVisible(true);
	}
}

void UBattleGridIndicatorComponent::ClearHovered()
{
	if (!ResolveGridManager())
	{
		return;
	}

	if (!GridManager->IsTilePresentationEnabled())
	{
		return;
	}

	for (const FHexOffsetCoord& Coord : GridManager->TargetGridArray)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (Tile)
		{
			Tile->SetTargetIndicatorVisible(false);
		}
	}

	GridManager->TargetGridArray.Empty();
}

void UBattleGridIndicatorComponent::ClearAllHovered()
{
	if (!ResolveGridManager())
	{
		return;
	}

	if (!GridManager->IsTilePresentationEnabled())
	{
		return;
	}

	for (FBattleGridCell& Cell : GridManager->GridCells)
	{
		if (Cell.TileActor)
		{
			Cell.TileActor->SetTargetIndicatorVisible(false);
		}
	}

	GridManager->TargetGridArray.Empty();
}

void UBattleGridIndicatorComponent::SetExchange(const EMuksiBattleCardType& AttackType, const TArray<FHexOffsetCoord>& Coords, bool bEnemy)
{
	if (!ResolveGridManager())
	{
		return;
	}

	if (!GridManager->IsTilePresentationEnabled())
	{
		return;
	}

	for (const FHexOffsetCoord& Coord : Coords)
	{
		ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);

		if (Tile)
		{
			Tile->SetExchangeIndicator(AttackType, bEnemy);
		}
	}
}

void UBattleGridIndicatorComponent::ClearExchange()
{
	if (!ResolveGridManager())
	{
		return;
	}

	if (!GridManager->IsTilePresentationEnabled())
	{
		return;
	}

	for (FBattleGridCell& Cell : GridManager->GridCells)
	{
		if (Cell.TileActor)
		{
			Cell.TileActor->ClearExchangeIndicator();
		}
	}
}