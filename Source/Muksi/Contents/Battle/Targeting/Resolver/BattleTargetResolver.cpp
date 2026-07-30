#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

namespace
{
	FHexCubeCoord RoundCube(float X, float Y, float Z)
	{
		int32 RoundedX = FMath::RoundToInt(X);
		int32 RoundedY = FMath::RoundToInt(Y);
		int32 RoundedZ = FMath::RoundToInt(Z);

		const float XDifference = FMath::Abs(static_cast<float>(RoundedX) - X);
		const float YDifference = FMath::Abs(static_cast<float>(RoundedY) - Y);
		const float ZDifference = FMath::Abs(static_cast<float>(RoundedZ) - Z);

		if (XDifference > YDifference && XDifference > ZDifference)
		{
			RoundedX = -RoundedY - RoundedZ;
		}
		else if (YDifference > ZDifference)
		{
			RoundedY = -RoundedX - RoundedZ;
		}
		else
		{
			RoundedZ = -RoundedX - RoundedY;
		}

		return FHexCubeCoord(RoundedX, RoundedY, RoundedZ);
	}
}

FTargetingIntent FBattleTargetResolver::CaptureIntent(const FTargetingResult& TargetingResult, const FHexOffsetCoord& SourceCoord)
{
	FTargetingIntent Intent;

	for (const FTargetingStepContext& StepContext : TargetingResult.StepContexts)
	{
		FTargetingStepIntent StepIntent;
		const FHexOffsetCoord OriginCoord = StepContext.HasOriginCoord() ? StepContext.OriginCoord : SourceCoord;

		StepIntent.bHasSelectedCoord = StepContext.HasSelectedCoord();
		StepIntent.Direction = StepContext.Direction;

		if (StepIntent.bHasSelectedCoord)
		{
			StepIntent.WorldSelectedCoord = StepContext.SelectedCoord;
			StepIntent.RelativeSelectedCube =
				FHexGridMath::OffsetToCube(StepContext.SelectedCoord) -
				FHexGridMath::OffsetToCube(OriginCoord);
		}

		Intent.Steps.Add(StepIntent);
	}

	if (!Intent.IsEmpty())
	{
		return Intent;
	}

	FHexOffsetCoord SelectedCoord = FHexOffsetCoord::Invalid();

	if (TargetingResult.HasSelectedCoord())
	{
		SelectedCoord = TargetingResult.GetSelectedCoord();
	}
	else if (!TargetingResult.AffectedCoords.IsEmpty())
	{
		SelectedCoord = TargetingResult.AffectedCoords[0];
	}

	if (!SelectedCoord.IsValid())
	{
		return Intent;
	}

	FTargetingStepIntent StepIntent;
	StepIntent.bHasSelectedCoord = true;
	StepIntent.WorldSelectedCoord = SelectedCoord;
	StepIntent.RelativeSelectedCube =
		FHexGridMath::OffsetToCube(SelectedCoord) -
		FHexGridMath::OffsetToCube(SourceCoord);
	Intent.Steps.Add(StepIntent);

	return Intent;
}

bool FBattleTargetResolver::ResolveAction(FBattleAction& InOutAction, ABattleGridManager* GridManager)
{
	if (!IsValid(InOutAction.Attacker) || !IsValid(InOutAction.Card) || !IsValid(GridManager))
	{
		return false;
	}

	if (InOutAction.TargetingIntent.IsEmpty())
	{
		InOutAction.TargetingIntent = CaptureIntent(InOutAction.TargetingResult, InOutAction.Attacker->GetCharacterPosition());
	}

	if (InOutAction.TargetingIntent.IsEmpty())
	{
		return true;
	}

	const FTargetingResult SourceResult = InOutAction.TargetingResult;
	FTargetingResult ResolvedResult;

	for (int32 StepIndex = 0; StepIndex < InOutAction.TargetingIntent.Steps.Num(); ++StepIndex)
	{
		const FTargetingStepIntent& StepIntent = InOutAction.TargetingIntent.Steps[StepIndex];
		const FTargetingStepCardData* StepData = InOutAction.Card->TargetingData.GetStep(StepIndex);

		FHexOffsetCoord OriginCoord;

		if (!ResolveStepOrigin(InOutAction, ResolvedResult, StepData, OriginCoord))
		{
			return false;
		}

		FTargetingStepContext ResolvedStepContext;
		ResolvedStepContext.Reset();
		ResolvedStepContext.OriginCoord = OriginCoord;
		ResolvedStepContext.OriginWorldLocation = GridManager->GetWorldLocationByCoord(OriginCoord);
		ResolvedStepContext.Direction = StepIntent.Direction;

		FHexOffsetCoord SelectedCoord;
		TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;
		const ETargetingIntentBinding IntentBinding = StepData
			? StepData->IntentBinding
			: ETargetingIntentBinding::SourceRelative;
		const bool bShouldResolveSelectedCoord =
			StepIntent.bHasSelectedCoord ||
			IntentBinding == ETargetingIntentBinding::TargetCharacter;

		if (bShouldResolveSelectedCoord)
		{
			if (!ResolveStepSelectedCoord(
				InOutAction,
				SourceResult,
				StepIntent,
				StepData,
				StepIndex,
				OriginCoord,
				GridManager,
				SelectedCoord,
				TargetCharacters
			))
			{
				return false;
			}

			ResolvedStepContext.SelectedCoord = SelectedCoord;
			ResolvedStepContext.SelectedWorldLocation = GridManager->GetWorldLocationByCoord(SelectedCoord);
		}

		if (IntentBinding != ETargetingIntentBinding::SourceRelative)
		{
			ResolvedStepContext.Direction = INDEX_NONE;
		}

		if (ResolvedStepContext.Direction != INDEX_NONE)
		{
			const FHexOffsetCoord AimCoord = FHexGridMath::GetNeighborCoord(OriginCoord, ResolvedStepContext.Direction);

			if (GridManager->IsValidCoord(AimCoord))
			{
				ResolvedStepContext.AimWorldLocation = GridManager->GetWorldLocationByCoord(AimCoord);
			}
		}
		else if (ResolvedStepContext.HasSelectedCoord())
		{
			const FVector AimDirection = ResolvedStepContext.SelectedWorldLocation - ResolvedStepContext.OriginWorldLocation;
			ResolvedStepContext.Direction = FHexGridMath::GetClosestDirectionByWorldVector(AimDirection);
			ResolvedStepContext.AimWorldLocation = ResolvedStepContext.SelectedWorldLocation;
		}

		ResolvedStepContext.TargetCharacters = TargetCharacters;
		ResolvedStepContext.bCanConfirm = true;
		ResolvedResult.StepContexts.Add(ResolvedStepContext);

		for (ABattleCharacterBase* TargetCharacter : TargetCharacters)
		{
			if (IsValid(TargetCharacter))
			{
				ResolvedResult.TargetCharacters.AddUnique(TargetCharacter);
			}
		}
	}

	InOutAction.TargetingResult = MoveTemp(ResolvedResult);
	return ApplyFinalPattern(InOutAction, GridManager);
}

bool FBattleTargetResolver::ResolveStepOrigin(
	const FBattleAction& Action,
	const FTargetingResult& ResolvedResult,
	const FTargetingStepCardData* StepData,
	FHexOffsetCoord& OutOriginCoord)
{
	const ETargetingOriginSource OriginSource = StepData
		? StepData->OriginSource
		: ETargetingOriginSource::SourceCharacter;

	switch (OriginSource)
	{
	case ETargetingOriginSource::PreviousStep:
	{
		const FTargetingStepContext* PreviousStep = ResolvedResult.GetLastStepContext();

		if (!PreviousStep || !PreviousStep->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = PreviousStep->SelectedCoord;
		return true;
	}

	case ETargetingOriginSource::SpecificStep:
	{
		const int32 OriginStepIndex = StepData ? StepData->OriginStepIndex : 0;
		const FTargetingStepContext* SpecificStep = ResolvedResult.GetStepContext(OriginStepIndex);

		if (!SpecificStep || !SpecificStep->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = SpecificStep->SelectedCoord;
		return true;
	}

	case ETargetingOriginSource::SourceCharacter:
	default:
		if (!IsValid(Action.Attacker))
		{
			return false;
		}

		OutOriginCoord = Action.Attacker->GetCharacterPosition();
		return OutOriginCoord.IsValid();
	}
}

bool FBattleTargetResolver::ResolveStepSelectedCoord(
	const FBattleAction& Action,
	const FTargetingResult& SourceResult,
	const FTargetingStepIntent& StepIntent,
	const FTargetingStepCardData* StepData,
	int32 StepIndex,
	const FHexOffsetCoord& OriginCoord,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutSelectedCoord,
	TArray<TObjectPtr<ABattleCharacterBase>>& OutTargetCharacters)
{
	const ETargetingIntentBinding IntentBinding = StepData
		? StepData->IntentBinding
		: ETargetingIntentBinding::SourceRelative;

	if (IntentBinding == ETargetingIntentBinding::TargetCharacter)
	{
		const FTargetingStepContext* SourceStepContext = SourceResult.GetStepContext(StepIndex);
		ABattleCharacterBase* TargetCharacter = SourceStepContext
			? SourceStepContext->GetPrimaryTargetCharacter()
			: SourceResult.GetPrimaryTargetCharacter();

		if (!IsValid(TargetCharacter))
		{
			return false;
		}

		OutSelectedCoord = TargetCharacter->GetCharacterPosition();

		if (!GridManager->IsValidCoord(OutSelectedCoord))
		{
			return false;
		}

		OutTargetCharacters.Add(TargetCharacter);
		return true;
	}

	FHexOffsetCoord DesiredCoord = StepIntent.WorldSelectedCoord;

	if (IntentBinding == ETargetingIntentBinding::SourceRelative)
	{
		const FHexCubeCoord DesiredCube =
			FHexGridMath::OffsetToCube(OriginCoord) +
			StepIntent.RelativeSelectedCube;
		DesiredCoord = FHexGridMath::CubeToOffset(DesiredCube);
	}

	return ResolveInvalidCoord(
		OriginCoord,
		DesiredCoord,
		StepData,
		Action.Attacker,
		GridManager,
		OutSelectedCoord
	);
}

bool FBattleTargetResolver::ResolveInvalidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	const FTargetingStepCardData* StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	if (IsCoordUsable(DesiredCoord, StepData, Attacker, GridManager))
	{
		OutResolvedCoord = DesiredCoord;
		return true;
	}

	const EInvalidTargetResolvePolicy ResolvePolicy = StepData
		? StepData->InvalidResolvePolicy
		: EInvalidTargetResolvePolicy::FindNearestValid;

	switch (ResolvePolicy)
	{
	case EInvalidTargetResolvePolicy::StopAtLastValid:
		return FindLastValidCoord(OriginCoord, DesiredCoord, StepData, Attacker, GridManager, OutResolvedCoord);

	case EInvalidTargetResolvePolicy::FindNearestValid:
		return FindNearestValidCoord(OriginCoord, DesiredCoord, StepData, Attacker, GridManager, OutResolvedCoord);

	case EInvalidTargetResolvePolicy::KeepResolvedCoord:
		if (GridManager->IsValidCoord(DesiredCoord))
		{
			OutResolvedCoord = DesiredCoord;
			return true;
		}
		return false;

	case EInvalidTargetResolvePolicy::Cancel:
	default:
		return false;
	}
}

bool FBattleTargetResolver::IsCoordUsable(
	const FHexOffsetCoord& Coord,
	const FTargetingStepCardData* StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager)
{
	if (!GridManager || !GridManager->IsValidCoord(Coord))
	{
		return false;
	}

	if (!StepData || !StepData->bRequireAvailableDestination)
	{
		return true;
	}

	UBattleGridNavigationComponent* Navigation = GridManager->GetNavigationComponent();
	return Navigation && Navigation->IsCellAvailable(Coord, Attacker);
}

bool FBattleTargetResolver::FindLastValidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	const FTargetingStepCardData* StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	const int32 StepCount = FHexGridMath::GetHexDistance(OriginCoord, DesiredCoord);
	FHexOffsetCoord LastValidCoord = OriginCoord;

	for (int32 StepIndex = 1; StepIndex <= StepCount; ++StepIndex)
	{
		const FHexOffsetCoord CandidateCoord = ResolveCubeLineCoord(OriginCoord, DesiredCoord, StepIndex, StepCount);

		if (!IsCoordUsable(CandidateCoord, StepData, Attacker, GridManager))
		{
			break;
		}

		LastValidCoord = CandidateCoord;
	}

	if (!IsCoordUsable(LastValidCoord, StepData, Attacker, GridManager))
	{
		return false;
	}

	OutResolvedCoord = LastValidCoord;
	return true;
}

bool FBattleTargetResolver::FindNearestValidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	const FTargetingStepCardData* StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	if (!GridManager)
	{
		return false;
	}

	const int32 MaximumDistance = FHexGridMath::GetHexDistance(OriginCoord, DesiredCoord);
	int32 BestDesiredDistance = MAX_int32;
	int32 BestOriginDistance = MAX_int32;
	UBattleGridNavigationComponent* Navigation = GridManager->GetNavigationComponent();

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CandidateCoord(X, Y);

			if (!IsCoordUsable(CandidateCoord, StepData, Attacker, GridManager))
			{
				continue;
			}

			const int32 OriginDistance = FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord);

			if (OriginDistance > MaximumDistance)
			{
				continue;
			}

			if (StepData && StepData->bRequireReachablePath)
			{
				TArray<FHexOffsetCoord> Path;

				if (CandidateCoord != OriginCoord &&
					(!Navigation || !Navigation->FindGroundPath(OriginCoord, CandidateCoord, Path, Attacker)))
				{
					continue;
				}
			}

			const int32 DesiredDistance = FHexGridMath::GetHexDistance(DesiredCoord, CandidateCoord);

			if (DesiredDistance > BestDesiredDistance)
			{
				continue;
			}

			if (DesiredDistance == BestDesiredDistance && OriginDistance >= BestOriginDistance)
			{
				continue;
			}

			BestDesiredDistance = DesiredDistance;
			BestOriginDistance = OriginDistance;
			OutResolvedCoord = CandidateCoord;
		}
	}

	return BestDesiredDistance != MAX_int32;
}

FHexOffsetCoord FBattleTargetResolver::ResolveCubeLineCoord(
	const FHexOffsetCoord& StartCoord,
	const FHexOffsetCoord& EndCoord,
	int32 StepIndex,
	int32 StepCount)
{
	if (StepCount <= 0)
	{
		return StartCoord;
	}

	const FHexCubeCoord StartCube = FHexGridMath::OffsetToCube(StartCoord);
	const FHexCubeCoord EndCube = FHexGridMath::OffsetToCube(EndCoord);
	const float Alpha = static_cast<float>(StepIndex) / static_cast<float>(StepCount);

	return FHexGridMath::CubeToOffset(
		RoundCube(
			FMath::Lerp(static_cast<float>(StartCube.X), static_cast<float>(EndCube.X), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Y), static_cast<float>(EndCube.Y), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Z), static_cast<float>(EndCube.Z), Alpha)
		)
	);
}

bool FBattleTargetResolver::ApplyFinalPattern(FBattleAction& InOutAction, ABattleGridManager* GridManager)
{
	InOutAction.TargetingResult.AffectedCoords.Empty();
	InOutAction.TargetingResult.PathCoords.Empty();

	const int32 LastStepIndex = InOutAction.TargetingResult.StepContexts.Num() - 1;
	const FTargetingStepCardData* StepData = InOutAction.Card->TargetingData.GetStep(LastStepIndex);

	if (!StepData || !StepData->PatternClass)
	{
		if (InOutAction.TargetingResult.HasSelectedCoord())
		{
			InOutAction.TargetingResult.AddAffectedCoord(InOutAction.TargetingResult.GetSelectedCoord());
		}

		return true;
	}

	const UAreaPattern* Pattern = StepData->PatternClass->GetDefaultObject<UAreaPattern>();

	if (!Pattern)
	{
		return false;
	}

	FAreaPatternContext PatternContext;
	PatternContext.SourceCharacter = InOutAction.Attacker;
	PatternContext.GridManager = GridManager;
	Pattern->ApplyPattern(PatternContext, StepData->PatternData, InOutAction.TargetingResult);
	return true;
}
