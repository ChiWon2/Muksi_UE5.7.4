#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePatternData.h"

void UConePattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord&, int32 Direction, TArray<FTargetingGroup>& OutGroups) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FConePatternData* Data = PatternData.GetPtr<FConePatternData>();

	if (!Data || !GridManager->IsValidCoord(OriginCoord) || Direction == INDEX_NONE)
		return;

	FTargetingGroup& Group = OutGroups.AddDefaulted_GetRef();
	const FHexCubeCoord OriginCube = FHexGridMath::OffsetToCube(OriginCoord);
	const int32 SafeRange = FMath::Max(1, Data->Range);
	const float SafeAngle = FMath::Clamp(Data->Angle, 1.0f, 360.0f);

	if (Data->bIncludeOriginCoord)
		AddAffectedCoord(Group.AffectedCoords, OriginCoord);

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CandidateCoord(X, Y);

			if (CandidateCoord == OriginCoord || !CandidateCoord.IsValid())
				continue;

			if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) > SafeRange)
				continue;

			const FHexCubeCoord RelativeCube = FHexGridMath::OffsetToCube(CandidateCoord) - OriginCube;

			if (!IsInsideHexCone(RelativeCube, Direction, SafeAngle))
				continue;

			AddAffectedCoord(Group.AffectedCoords, CandidateCoord);
		}
	}
}

const UScriptStruct* UConePattern::GetPatternDataStruct() const
{
	return FConePatternData::StaticStruct();
}

bool UConePattern::IsInsideHexCone(const FHexCubeCoord& RelativeCube, const int32 Direction, const float ConeAngle) const
{
	if (ConeAngle >= 360.0f - KINDA_SMALL_NUMBER)
		return true;

	FVector2D CandidateDirection = FHexGridMath::GetGridVector2D(RelativeCube);
	FVector2D ConeDirection = FHexGridMath::GetGridVector2D(FHexGridMath::GetCubeDirection(Direction));

	if (!CandidateDirection.Normalize() || !ConeDirection.Normalize())
		return false;

	const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(ConeAngle * 0.5f));
	return FVector2D::DotProduct(ConeDirection, CandidateDirection) >= MinimumDot;
}
