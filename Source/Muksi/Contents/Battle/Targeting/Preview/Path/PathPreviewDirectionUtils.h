#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"

namespace MuksiPathPreview
{
	inline FVector SnapDirectionToEightDirections(const FVector& Direction)
	{
		const float Yaw = Direction.Rotation().Yaw;
		const float SnappedYaw = FMath::GridSnap(Yaw, 45.0f);
		return FRotator(0.0f, SnappedYaw, 0.0f).Vector();
	}

	inline bool GetGroupEndLocation(const FTargetingPreviewContext& Context, const FTargetingGroup& Group, FVector& OutLocation)
	{
		if (!Group.PathCoords.IsEmpty())
			return Context.GridManager->GetPresentationWorldLocationByCoord(Group.PathCoords.Last(), OutLocation);

		if (!Context.HasTargetCoord())
			return false;

		return Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetTargetCoord(), OutLocation);
	}

	inline bool GetSixDirectionWorldVector(const FTargetingPreviewContext& Context, const FTargetingGroup& Group, const FVector& StartLocation, FVector& OutDirection)
	{
		int32 Direction = Group.Direction;

		if (Direction == INDEX_NONE && Context.HasDirection())
			Direction = Context.GetDirection();

		if (Direction == INDEX_NONE)
			return false;

		FVector NeighborLocation = FVector::ZeroVector;
		const FHexOffsetCoord NeighborCoord = FHexGridMath::GetNeighborCoord(Context.GetOriginCoord(), Direction);

		if (!Context.GridManager->GetPresentationWorldLocationByCoord(NeighborCoord, NeighborLocation))
			return false;

		OutDirection = NeighborLocation - StartLocation;
		OutDirection.Z = 0.0f;
		return OutDirection.Normalize();
	}

	inline bool GetPathDirection(const FTargetingPreviewContext& Context, const FTargetingGroup& Group, const FPathPreviewData& Data, const FVector& StartLocation, const FVector& RawEndLocation, FVector& OutDirection)
	{
		if (Data.DirectionMode == EPathPreviewDirectionMode::SixDirections && GetSixDirectionWorldVector(Context, Group, StartLocation, OutDirection))
			return true;

		OutDirection = RawEndLocation - StartLocation;
		OutDirection.Z = 0.0f;

		if (!OutDirection.Normalize())
			return false;

		if (Data.DirectionMode == EPathPreviewDirectionMode::EightDirections)
			OutDirection = SnapDirectionToEightDirections(OutDirection);

		return true;
	}
}
