#include "Muksi/Contents/Battle/Targeting/Selection/Direction/DirectionSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Direction/DirectionSelectionData.h"

void UDirectionSelection::Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const
{
	InitializeStepContext(Context, OutStepContext);

	TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(Context, SelectionData);

	const FDirectionSelectionData* Data = SelectionData.GetPtr<FDirectionSelectionData>();

	if (!Data)
	{
		return;
	}

	FVector AimDirection = Context.InputContext.AimWorldLocation - Context.OriginWorldLocation;
	AimDirection.Z = 0.0f;

	const float MinimumAimDistance = FMath::Max(0.0f, Data->MinimumAimDistance);

	if (AimDirection.SizeSquared() < FMath::Square(MinimumAimDistance))
	{
		return;
	}

	if (!AimDirection.Normalize())
	{
		return;
	}

	OutStepContext.Direction = ResolveHexDirection(Context, AimDirection);
	OutStepContext.SelectedCoord = Context.InputContext.HoveredCoord;
	OutStepContext.SelectedWorldLocation = Context.InputContext.AimWorldLocation;
	OutStepContext.TargetCharacters = Context.InputContext.CandidateCharacters;
	OutStepContext.bCanConfirm = OutStepContext.HasDirection();

	if (!Context.InputContext.HasHoveredCoord())
	{
		return;
	}

	const ABattleGridTile* SelectedTile = Context.GridManager->GetTileByCoord(Context.InputContext.HoveredCoord);

	if (SelectedTile)
	{
		OutStepContext.SelectedWorldLocation = SelectedTile->GetGridCenterWorldLocation();
	}
}

const UScriptStruct* UDirectionSelection::GetSelectionDataStruct() const
{
	return FDirectionSelectionData::StaticStruct();
}

int32 UDirectionSelection::ResolveHexDirection(const FTargetSelectionContext& Context, const FVector& AimDirection) const
{
	if (!Context.GridManager)
	{
		return FHexGridMath::GetClosestDirectionByWorldVector(AimDirection);
	}

	int32 BestDirection = INDEX_NONE;
	float BestDirectionDot = -1.0f;

	for (int32 DirectionIndex = 0; DirectionIndex < FHexGridMath::DirectionCount; ++DirectionIndex)
	{
		const FIntPoint NeighborCoord = FHexGridMath::GetNeighborCoord(Context.OriginCoord, DirectionIndex);

		if (!Context.GridManager->IsValidGridCoord(NeighborCoord))
		{
			continue;
		}

		const ABattleGridTile* NeighborTile = Context.GridManager->GetTileByCoord(NeighborCoord);

		if (!NeighborTile)
		{
			continue;
		}

		FVector NeighborDirection = NeighborTile->GetGridCenterWorldLocation() - Context.OriginWorldLocation;
		NeighborDirection.Z = 0.0f;

		if (!NeighborDirection.Normalize())
		{
			continue;
		}

		const float DirectionDot = FVector::DotProduct(AimDirection, NeighborDirection);

		if (DirectionDot <= BestDirectionDot)
		{
			continue;
		}

		BestDirectionDot = DirectionDot;
		BestDirection = DirectionIndex;
	}

	if (BestDirection != INDEX_NONE)
	{
		return BestDirection;
	}

	return FHexGridMath::GetClosestDirectionByWorldVector(AimDirection);
}