#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Condition/TargetingConditionService.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Identity/TargetingIdentityProvider.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"
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

bool FBattleTargetResolver::ResolveAction(const FBattleAction& Action, ABattleGridManager* GridManager, FResolvedTargeting& OutResolvedTargeting, const FTargetingCardData* TargetingDataOverride)
{
	OutResolvedTargeting.Reset();

	if (!IsValid(Action.Attacker) || !IsValid(Action.Card) || !IsValid(GridManager))
	{
		return false;
	}

	const FTargetingCardData& TargetingData = TargetingDataOverride ? *TargetingDataOverride : Action.Card->TargetingData;
	return ResolveIntent(Action.Attacker, GridManager, TargetingData, Action.TargetingIntent, OutResolvedTargeting);
}


bool FBattleTargetResolver::ResolveActionThroughStep(
	const FBattleAction& Action,
	ABattleGridManager* GridManager,
	int32 LastStepIndex,
	FResolvedTargeting& OutResolvedTargeting,
	const FTargetingCardData* TargetingDataOverride)
{
	OutResolvedTargeting.Reset();

	if (!IsValid(Action.Attacker) || !IsValid(Action.Card) || !IsValid(GridManager))
	{
		return false;
	}

	const FTargetingCardData& SourceTargetingData = TargetingDataOverride ? *TargetingDataOverride : Action.Card->TargetingData;
	if (!SourceTargetingData.Steps.IsValidIndex(LastStepIndex) || !Action.TargetingIntent.Steps.IsValidIndex(LastStepIndex))
	{
		return false;
	}

	FTargetingCardData PartialTargetingData;
	PartialTargetingData.Steps.Append(SourceTargetingData.Steps.GetData(), LastStepIndex + 1);

	FTargetingIntent PartialIntent;
	PartialIntent.Steps.Append(Action.TargetingIntent.Steps.GetData(), LastStepIndex + 1);

	return ResolveIntent(Action.Attacker, GridManager, PartialTargetingData, PartialIntent, OutResolvedTargeting);
}

bool FBattleTargetResolver::ResolveIntent(
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	const FTargetingCardData& TargetingData,
	const FTargetingIntent& TargetingIntent,
	FResolvedTargeting& OutResolvedTargeting)
{
	OutResolvedTargeting.Reset();

	if (!IsValid(Attacker) || !IsValid(GridManager))
	{
		return false;
	}

	if (!TargetingData.HasSteps())
	{
		return TargetingIntent.IsEmpty();
	}

	if (TargetingIntent.Steps.Num() != TargetingData.Steps.Num())
	{
		return false;
	}

	for (int32 StepIndex = 0; StepIndex < TargetingData.Steps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData& StepData = TargetingData.Steps[StepIndex];
		const FTargetingStepIntent& StepIntent = TargetingIntent.Steps[StepIndex];
		FHexOffsetCoord OriginCoord;

		if (!ResolveStepOrigin(Attacker, OutResolvedTargeting, StepData, OriginCoord))
		{
			return false;
		}

		FHexOffsetCoord DesiredCoord;

		if (!ResolveDesiredCoord(StepIntent, StepData, OriginCoord, GridManager, DesiredCoord))
		{
			return false;
		}

		FHexOffsetCoord ResolvedCoord;
		// IntentBinding은 좌표 재해석 방식만 결정한다. Targeting 시점에 확정된
		// 방향은 TargetCharacter / WorldFixed에서도 폐기하지 않고 그대로 사용한다.
		const int32 ResolveDirection = StepIntent.Direction;

		if (!ResolveInvalidCoord(OriginCoord, DesiredCoord, ResolveDirection, StepData, Attacker, GridManager, ResolvedCoord))
		{
			return false;
		}

		FTargetingStepResult ResolvedStep;

		if (!EvaluateStepAtCoord(OriginCoord, ResolvedCoord, ResolveDirection, StepData, Attacker, GridManager, ResolvedStep))
		{
			return false;
		}

		// 현재 단계가 PreviousStep를 원점으로 사용하고 Origin == Selected라면
		// 좌표만으로는 방향을 만들 수 없다. Intent에도 방향이 없는 구형 데이터나
		// 직접 생성된 AI Intent를 위해 직전 Resolve 결과의 방향을 fallback으로 쓴다.
		if (!ResolvedStep.HasDirection() &&
			StepData.Selection.OriginSource == ETargetingOriginSource::PreviousStep)
		{
			const FTargetingStepResult* PreviousStep = OutResolvedTargeting.GetLastStep();

			if (PreviousStep && PreviousStep->HasDirection())
			{
				ResolvedStep.Direction = PreviousStep->Direction;
			}
		}

		OutResolvedTargeting.Steps.Add(ResolvedStep);
	}

	if (!ApplyFinalPattern(GridManager, TargetingData, OutResolvedTargeting))
	{
		return false;
	}

	return true;
}

bool FBattleTargetResolver::ResolveStepOrigin(
	ABattleCharacterBase* Attacker,
	const FResolvedTargeting& ResolvedTargeting,
	const FTargetingStepCardData& StepData,
	FHexOffsetCoord& OutOriginCoord)
{
	switch (StepData.Selection.OriginSource)
	{
	case ETargetingOriginSource::PreviousStep:
	{
		const FTargetingStepResult* PreviousStep = ResolvedTargeting.GetLastStep();

		if (!PreviousStep || !PreviousStep->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = PreviousStep->SelectedCoord;
		return true;
	}

	case ETargetingOriginSource::SpecificStep:
	{
		const FTargetingStepResult* SpecificStep = ResolvedTargeting.GetStep(StepData.Selection.OriginStepIndex);

		if (!SpecificStep || !SpecificStep->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = SpecificStep->SelectedCoord;
		return true;
	}

	case ETargetingOriginSource::SourceCharacter:
	default:
		if (!IsValid(Attacker))
		{
			return false;
		}
		OutOriginCoord = Attacker->GetCharacterCoord();
		return OutOriginCoord.IsValid();
	}
}

bool FBattleTargetResolver::ResolveDesiredCoord(
	const FTargetingStepIntent& StepIntent,
	const FTargetingStepCardData& StepData,
	const FHexOffsetCoord& OriginCoord,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutDesiredCoord)
{
	OutDesiredCoord = FHexOffsetCoord::Invalid();

	switch (StepData.AdvancedSettings.IntentBinding)
	{
	case ETargetingIntentBinding::TargetCharacter:
		if (ABattleCharacterBase* BoundTargetCharacter = FTargetingCharacterIdentity::FindCharacterByKey(GridManager, StepIntent.TargetCharacterKey))
		{
			OutDesiredCoord = BoundTargetCharacter->GetCharacterCoord();
			break;
		}

		if (!StepIntent.HasSelectedCoord())
		{
			return false;
		}

		OutDesiredCoord = StepIntent.SelectedCoord;
		break;

	case ETargetingIntentBinding::WorldFixed:
		if (!StepIntent.HasSelectedCoord())
		{
			return false;
		}
		OutDesiredCoord = StepIntent.SelectedCoord;
		break;

	case ETargetingIntentBinding::SourceRelative:
	default:
		if (StepIntent.HasSelectedCoord())
		{
			OutDesiredCoord = FHexGridMath::CubeToOffset(FHexGridMath::OffsetToCube(OriginCoord) + StepIntent.RelativeOffset);
		}
		else if (StepIntent.Direction != INDEX_NONE)
		{
			OutDesiredCoord = FHexGridMath::GetNeighborCoord(OriginCoord, StepIntent.Direction);
		}
		else
		{
			return false;
		}
		break;
	}

	return OutDesiredCoord.IsValid();
}

bool FBattleTargetResolver::ResolveInvalidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	int32 Direction,
	const FTargetingStepCardData& StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	if (IsCoordUsable(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager))
	{
		OutResolvedCoord = DesiredCoord;
		return true;
	}

	switch (StepData.AdvancedSettings.InvalidResolvePolicy)
	{
	case EInvalidTargetResolvePolicy::StopAtLastValid:
		return FindLastValidCoord(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager, OutResolvedCoord);

	case EInvalidTargetResolvePolicy::FindNearestValid:
		return FindNearestValidCoord(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager, OutResolvedCoord);

	case EInvalidTargetResolvePolicy::Cancel:
	default:
		return false;
	}
}

bool FBattleTargetResolver::EvaluateStepAtCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& CandidateCoord,
	int32 Direction,
	const FTargetingStepCardData& StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FTargetingStepResult& OutStepResult)
{
	OutStepResult.Reset();

	if (!IsValid(Attacker) || !IsValid(GridManager) || !StepData.Selection.SelectionClass || !GridManager->IsValidCoord(OriginCoord) || !GridManager->IsValidCoord(CandidateCoord))
	{
		return false;
	}

	const UTargetSelection* Selection = StepData.Selection.SelectionClass->GetDefaultObject<UTargetSelection>();

	if (!Selection)
	{
		return false;
	}

	Selection->EvaluateCandidate(GridManager, OriginCoord, CandidateCoord, StepData.Selection.SelectionData, OutStepResult);

	if (!OutStepResult.bValid)
	{
		return false;
	}

	if (Direction != INDEX_NONE)
	{
		OutStepResult.Direction = Direction;
	}
	else if (!OutStepResult.HasDirection() && OutStepResult.HasSelectedCoord())
	{
		FVector AimDirection = GridManager->GetWorldLocationByCoord(OutStepResult.SelectedCoord) - GridManager->GetWorldLocationByCoord(OutStepResult.OriginCoord);
		AimDirection.Z = 0.0f;

		if (!AimDirection.IsNearlyZero())
		{
			OutStepResult.Direction = FHexGridMath::GetClosestDirectionByWorldVector(AimDirection);
		}
	}

	return true;
}

bool FBattleTargetResolver::IsCoordUsable(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& Coord,
	int32 Direction,
	const FTargetingStepCardData& StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager)
{
	FTargetingStepResult EvaluatedStep;

	if (!EvaluateStepAtCoord(OriginCoord, Coord, Direction, StepData, Attacker, GridManager, EvaluatedStep))
	{
		return false;
	}

	const UTargetSelection* Selection = StepData.Selection.SelectionClass
		? StepData.Selection.SelectionClass->GetDefaultObject<UTargetSelection>()
		: nullptr;

	FTargetingConditionRequest Request;
	Request.SourceCharacter = Attacker;
	Request.GridManager = GridManager;
	Request.OriginCoord = OriginCoord;
	Request.CandidateCoord = Coord;
	Request.ResolvedCoord = Coord;
	Request.Phase = ETargetingConditionPhase::Resolve;

	if (!UTargetingConditionService::Evaluate(StepData.Conditions.Resolve, Request))
	{
		return false;
	}

	return true;
}

bool FBattleTargetResolver::FindLastValidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	int32 Direction,
	const FTargetingStepCardData& StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	const int32 StepCount = FHexGridMath::GetHexDistance(OriginCoord, DesiredCoord);
	FHexOffsetCoord LastValidCoord = OriginCoord;
	bool bFoundValidCoord = false;

	for (int32 LineStepIndex = 1; LineStepIndex <= StepCount; ++LineStepIndex)
	{
		const FHexOffsetCoord CandidateCoord = ResolveCubeLineCoord(OriginCoord, DesiredCoord, LineStepIndex, StepCount);

		if (!IsCoordUsable(OriginCoord, CandidateCoord, Direction, StepData, Attacker, GridManager))
		{
			break;
		}

		LastValidCoord = CandidateCoord;
		bFoundValidCoord = true;
	}

	if (!bFoundValidCoord)
	{
		return false;
	}

	OutResolvedCoord = LastValidCoord;
	return true;
}

bool FBattleTargetResolver::FindNearestValidCoord(
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& DesiredCoord,
	int32 Direction,
	const FTargetingStepCardData& StepData,
	ABattleCharacterBase* Attacker,
	ABattleGridManager* GridManager,
	FHexOffsetCoord& OutResolvedCoord)
{
	if (!IsValid(GridManager))
	{
		return false;
	}

	int32 BestDesiredDistance = MAX_int32;
	int32 BestOriginDistance = MAX_int32;

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CandidateCoord(X, Y);

			if (!IsCoordUsable(OriginCoord, CandidateCoord, Direction, StepData, Attacker, GridManager))
			{
				continue;
			}

			const int32 DesiredDistance = FHexGridMath::GetHexDistance(DesiredCoord, CandidateCoord);
			const int32 OriginDistance = FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord);

			if (DesiredDistance > BestDesiredDistance || (DesiredDistance == BestDesiredDistance && OriginDistance >= BestOriginDistance))
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
	int32 LinePointIndex,
	int32 LinePointCount)
{
	if (LinePointCount <= 0)
	{
		return StartCoord;
	}

	const FHexCubeCoord StartCube = FHexGridMath::OffsetToCube(StartCoord);
	const FHexCubeCoord EndCube = FHexGridMath::OffsetToCube(EndCoord);
	const float Alpha = static_cast<float>(LinePointIndex) / static_cast<float>(LinePointCount);
	return FHexGridMath::CubeToOffset(
		RoundCube(
			FMath::Lerp(static_cast<float>(StartCube.X), static_cast<float>(EndCube.X), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Y), static_cast<float>(EndCube.Y), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Z), static_cast<float>(EndCube.Z), Alpha)
		)
	);
}

bool FBattleTargetResolver::ApplyFinalPattern(
	ABattleGridManager* GridManager,
	const FTargetingCardData& TargetingData,
	FResolvedTargeting& InOutResolvedTargeting)
{
	InOutResolvedTargeting.AffectedCoords.Empty();
	InOutResolvedTargeting.PathCoords.Empty();

	const int32 LastStepIndex = InOutResolvedTargeting.Steps.Num() - 1;
	const FTargetingStepCardData* StepData = TargetingData.GetStep(LastStepIndex);

	if (!StepData)
	{
		return false;
	}

	if (!StepData->Pattern.PatternClass)
	{
		if (InOutResolvedTargeting.HasSelectedCoord())
		{
			InOutResolvedTargeting.AddAffectedCoord(InOutResolvedTargeting.GetSelectedCoord());
		}

		return true;
	}

	const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();

	if (!Pattern)
	{
		return false;
	}

	Pattern->ApplyPattern(GridManager, StepData->Pattern.PatternData, InOutResolvedTargeting);
	return true;
}
