#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

namespace MuksiPathPreview
{
	inline bool GetPathRangeEndLocation(const FTargetingPreviewContext& Context, const FTargetingGroup& Group, const FVector& StartLocation, FVector& OutEndLocation)
	{
		if (Group.PathRange <= 0)
			return false;

		int32 Direction = Group.Direction;

		if (Direction == INDEX_NONE && Context.HasDirection())
			Direction = Context.GetDirection();

		if (Direction == INDEX_NONE)
			return false;

		const FHexOffsetCoord OriginCoord = Context.GetOriginCoord();
		const FHexOffsetCoord EndCoord = FHexGridMath::GetNeighborCoord(OriginCoord, Direction, Group.PathRange);
		const FVector GridStartLocation = Context.GridManager->GetWorldLocationByCoord(OriginCoord);
		const FVector GridEndLocation = Context.GridManager->GetWorldLocationByCoord(EndCoord);
		FVector PathOffset = GridEndLocation - GridStartLocation;
		PathOffset.Z = 0.0f;

		if (PathOffset.IsNearlyZero())
			return false;

		OutEndLocation = StartLocation + PathOffset;
		return true;
	}
}
