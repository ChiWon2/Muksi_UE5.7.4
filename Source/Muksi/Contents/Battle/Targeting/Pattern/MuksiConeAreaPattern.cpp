#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiConeAreaPattern.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Targeting/Data/Pattern/MuksiConeAreaPatternData.h"

void UMuksiConeAreaPattern::ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const
{
	if (!Context.SourceCharacter)
	{
		return;
	}

	if (!Context.GridManager)
	{
		return;
	}

	if (!IsAreaPatternDataValid(PatternData))
	{
		return;
	}

	const FMuksiConeAreaPatternData* Data = PatternData.GetPtr<FMuksiConeAreaPatternData>();

	if (!Data)
	{
		return;
	}

	const FIntPoint SourceCoord = Context.SourceCharacter->GetCharacterPosition();

	if (!Context.GridManager->IsValidGridCoord(SourceCoord))
	{
		return;
	}

	const FVector SourceWorldLocation = Context.SourceCharacter->GetActorLocation();
	const FVector AimWorldLocation = InOutResult.AimWorldLocation;
	const FVector FlatAimDirection(AimWorldLocation.X - SourceWorldLocation.X, AimWorldLocation.Y - SourceWorldLocation.Y, 0.0f);

	if (FlatAimDirection.IsNearlyZero())
	{
		return;
	}

	const int32 SafeRange = FMath::Max(1, Data->Range);
	const float SafeAngle = FMath::Clamp(Data->Angle, 1.0f, 360.0f);
	const FCubeCoord SourceCube = Context.GridManager->OddQToCube(SourceCoord);

	if (Data->bIncludeSourceCoord)
	{
		AddAffectedCoord(InOutResult, SourceCoord);
	}

	for (int32 X = 0; X < Context.GridManager->GridWidth; ++X)
	{
		for (int32 Y = 0; Y < Context.GridManager->GridHeight; ++Y)
		{
			const FIntPoint CandidateCoord(X, Y);

			if (CandidateCoord == SourceCoord)
			{
				continue;
			}

			if (!Context.GridManager->IsValidGridCoord(CandidateCoord))
			{
				continue;
			}

			const FCubeCoord CandidateCube = Context.GridManager->OddQToCube(CandidateCoord);

			if (GetCubeDistance(SourceCube, CandidateCube) > SafeRange)
			{
				continue;
			}

			const ABattleGridTile* CandidateTile = Context.GridManager->GetTileByCoord(CandidateCoord);

			if (!CandidateTile)
			{
				continue;
			}

			const FVector CandidateWorldLocation = CandidateTile->GetGridCenterWorldLocation();

			if (!IsDirectionInsideCone(SourceWorldLocation, AimWorldLocation, CandidateWorldLocation, SafeAngle))
			{
				continue;
			}

			AddAffectedCoord(InOutResult, CandidateCoord);
		}
	}
}

const UScriptStruct* UMuksiConeAreaPattern::GetAreaPatternDataStruct() const
{
	return FMuksiConeAreaPatternData::StaticStruct();
}

int32 UMuksiConeAreaPattern::GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B) const
{
	const int32 DeltaX = FMath::Abs(A.X - B.X);
	const int32 DeltaY = FMath::Abs(A.Y - B.Y);
	const int32 DeltaZ = FMath::Abs(A.Z - B.Z);

	return FMath::Max3(DeltaX, DeltaY, DeltaZ);
}

bool UMuksiConeAreaPattern::IsDirectionInsideCone(const FVector& SourceWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const
{
	FVector AimDirection = AimWorldLocation - SourceWorldLocation;
	FVector CandidateDirection = CandidateWorldLocation - SourceWorldLocation;

	AimDirection.Z = 0.0f;
	CandidateDirection.Z = 0.0f;

	if (!AimDirection.Normalize() || !CandidateDirection.Normalize())
	{
		return false;
	}

	const float HalfAngle = FMath::Clamp(ConeAngle * 0.5f, 0.5f, 180.0f);
	const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(HalfAngle));
	const float DirectionDot = FVector::DotProduct(AimDirection, CandidateDirection);

	return DirectionDot >= MinimumDot;
}