#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePatternData.h"

void UConePattern::ApplyPattern(ABattleGridManager* GridManager, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FConePatternData* Data = PatternData.GetPtr<FConePatternData>();
	const FTargetingStepResult* StepResult = InOutResult.GetLastStep();

	if (!Data || !StepResult || !StepResult->HasOriginCoord() || !StepResult->HasDirection())
	{
		return;
	}

	const FHexOffsetCoord OriginCoord = StepResult->OriginCoord;
	const FHexOffsetCoord AimCoord = FHexGridMath::GetNeighborCoord(OriginCoord, StepResult->Direction);

	if (!GridManager->IsValidCoord(OriginCoord))
	{
		return;
	}

	const FVector OriginWorldLocation = GridManager->GetWorldLocationByCoord(OriginCoord);
	const FVector AimWorldLocation = GridManager->GetWorldLocationByCoord(AimCoord);
	FVector AimDirection = AimWorldLocation - OriginWorldLocation;

	AimDirection.Z = 0.0f;

	if (AimDirection.IsNearlyZero())
	{
		return;
	}

	const int32 SafeRange = FMath::Max(1, Data->Range);
	const float SafeAngle = FMath::Clamp(Data->Angle, 1.0f, 360.0f);

	if (Data->bIncludeOriginCoord)
	{
		AddAffectedCoord(InOutResult, OriginCoord);
	}

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CandidateCoord(X, Y);

			if (CandidateCoord == OriginCoord)
			{
				continue;
			}

			if (!CandidateCoord.IsValid())
			{
				continue;
			}

			if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) > SafeRange)
			{
				continue;
			}

			const FVector CandidateWorldLocation = GridManager->GetWorldLocationByCoord(CandidateCoord);

			if (!IsInsideCone(OriginWorldLocation, AimWorldLocation, CandidateWorldLocation, SafeAngle))
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
