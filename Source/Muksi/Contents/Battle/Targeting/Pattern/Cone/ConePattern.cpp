#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePatternData.h"

void UConePattern::ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData);

	const FConePatternData* Data = PatternData.GetPtr<FConePatternData>();
	const FTargetingStepContext* StepContext = InOutResult.GetLastStepContext();

	if (!Data || !StepContext || !StepContext->HasOriginCoord())
	{
		return;
	}

	const FIntPoint OriginCoord = StepContext->OriginCoord;
	const FVector OriginWorldLocation = StepContext->OriginWorldLocation;
	const FVector AimWorldLocation = StepContext->AimWorldLocation;
	FVector AimDirection = AimWorldLocation - OriginWorldLocation;

	AimDirection.Z = 0.0f;

	if (!Context.GridManager->IsValidGridCoord(OriginCoord) || AimDirection.IsNearlyZero())
	{
		return;
	}

	const int32 SafeRange = FMath::Max(1, Data->Range);
	const float SafeAngle = FMath::Clamp(Data->Angle, 1.0f, 360.0f);

	if (Data->bIncludeOriginCoord)
	{
		AddAffectedCoord(InOutResult, OriginCoord);
	}

	for (int32 X = 0; X < Context.GridManager->GridWidth; ++X)
	{
		for (int32 Y = 0; Y < Context.GridManager->GridHeight; ++Y)
		{
			const FIntPoint CandidateCoord(X, Y);

			if (CandidateCoord == OriginCoord)
			{
				continue;
			}

			if (!Context.GridManager->IsValidGridCoord(CandidateCoord))
			{
				continue;
			}

			if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) > SafeRange)
			{
				continue;
			}

			const ABattleGridTile* CandidateTile = Context.GridManager->GetTileByCoord(CandidateCoord);

			if (!CandidateTile)
			{
				continue;
			}

			if (!IsInsideCone(OriginWorldLocation, AimWorldLocation, CandidateTile->GetGridCenterWorldLocation(), SafeAngle))
			{
				continue;
			}

			AddAffectedCoord(InOutResult, CandidateCoord);
		}
	}
}

const UScriptStruct* UConePattern::GetPatternDataStruct() const
{
	return FConePatternData::StaticStruct();
}

bool UConePattern::IsInsideCone(const FVector& OriginWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const
{
	if (ConeAngle >= 360.0f - KINDA_SMALL_NUMBER)
	{
		return true;
	}

	FVector AimDirection = AimWorldLocation - OriginWorldLocation;
	FVector CandidateDirection = CandidateWorldLocation - OriginWorldLocation;

	AimDirection.Z = 0.0f;
	CandidateDirection.Z = 0.0f;

	if (!AimDirection.Normalize() || !CandidateDirection.Normalize())
	{
		return false;
	}

	const float HalfAngle = ConeAngle * 0.5f;
	const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(HalfAngle));
	const float DirectionDot = FVector::DotProduct(AimDirection, CandidateDirection);

	return DirectionDot >= MinimumDot;
}