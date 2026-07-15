#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingGridPreviewRenderer.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingGridPreviewRenderer::SetGridManager(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

void UMuksiTargetingGridPreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command)
{
	if (!GridManager)
	{
		return;
	}

	GridManager->AllClearGridHovered();

	switch (Command.GridStyle)
	{
	case EMuksiCardTargetingGridPreviewStyle::None:
		return;

	case EMuksiCardTargetingGridPreviewStyle::AffectedTiles:
		GridManager->SetGridHovered(Command.AffectedCoords);
		return;

	case EMuksiCardTargetingGridPreviewStyle::PathTiles:
		GridManager->SetGridHovered(Command.PathCoords);
		return;

	case EMuksiCardTargetingGridPreviewStyle::AffectedAndPathTiles:
		GridManager->SetGridHovered(CombineCoords(Command.AffectedCoords, Command.PathCoords));
		return;

	default:
		return;
	}
}

void UMuksiTargetingGridPreviewRenderer::Hide()
{
	if (GridManager)
	{
		GridManager->AllClearGridHovered();
	}
}

TArray<FIntPoint> UMuksiTargetingGridPreviewRenderer::CombineCoords(const TArray<FIntPoint>& A, const TArray<FIntPoint>& B) const
{
	TArray<FIntPoint> Result = A;

	for (const FIntPoint& Coord : B)
	{
		Result.AddUnique(Coord);
	}

	return Result;
}