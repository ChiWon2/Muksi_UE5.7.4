#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Condition/TargetingConditionService.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

namespace
{
	ABattleCharacterBase* FindCharacterByTargetingKey(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FName CharacterKey)
	{
		if (!IsValid(GridManager) || CharacterKey.IsNone()) return nullptr;
		for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
		{
			for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
			{
				const FBattleGridCell* Cell = GridManager->GetCellByCoord(WorldType, FHexOffsetCoord(X, Y));
				ABattleCharacterBase* Character = Cell ? Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()) : nullptr;
				if (IsValid(Character) && Character->GetTargetingCharacterKey() == CharacterKey) return Character;
			}
		}
		return nullptr;
	}

	FHexCubeCoord RoundCube(float X, float Y, float Z)
	{
		int32 RoundedX = FMath::RoundToInt(X);
		int32 RoundedY = FMath::RoundToInt(Y);
		int32 RoundedZ = FMath::RoundToInt(Z);
		const float XDifference = FMath::Abs(static_cast<float>(RoundedX) - X);
		const float YDifference = FMath::Abs(static_cast<float>(RoundedY) - Y);
		const float ZDifference = FMath::Abs(static_cast<float>(RoundedZ) - Z);

		if (XDifference > YDifference && XDifference > ZDifference) RoundedX = -RoundedY - RoundedZ;
		else if (YDifference > ZDifference) RoundedY = -RoundedX - RoundedZ;
		else RoundedZ = -RoundedX - RoundedY;

		return FHexCubeCoord(RoundedX, RoundedY, RoundedZ);
	}

	FHexOffsetCoord CalculateHexLineCoord(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& EndCoord, int32 LinePointIndex, int32 LinePointCount)
	{
		if (LinePointCount <= 0) return StartCoord;
		const FHexCubeCoord StartCube = FHexGridMath::OffsetToCube(StartCoord);
		const FHexCubeCoord EndCube = FHexGridMath::OffsetToCube(EndCoord);
		const float Alpha = static_cast<float>(LinePointIndex) / static_cast<float>(LinePointCount);
		return FHexGridMath::CubeToOffset(RoundCube(
			FMath::Lerp(static_cast<float>(StartCube.X), static_cast<float>(EndCube.X), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Y), static_cast<float>(EndCube.Y), Alpha),
			FMath::Lerp(static_cast<float>(StartCube.Z), static_cast<float>(EndCube.Z), Alpha)));
	}
}

bool FBattleTargetResolver::ResolveIntent(ABattleCharacterBase* Attacker, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FTargetingCardData& TargetingData, const FTargetingIntent& TargetingIntent, TArray<FTargetingStep>& OutResolvedSteps)
{
	OutResolvedSteps.Reset();
	if (!IsValid(Attacker) || !IsValid(GridManager)) return false;
	if (!TargetingData.HasSteps()) return TargetingIntent.IsEmpty();
	if (TargetingIntent.Steps.Num() != TargetingData.Steps.Num()) return false;

	for (int32 StepIndex = 0; StepIndex < TargetingData.Steps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData& StepData = TargetingData.Steps[StepIndex];
		const FTargetingStepIntent& StepIntent = TargetingIntent.Steps[StepIndex];
		FHexOffsetCoord OriginCoord;
		if (!ResolveStepOrigin(Attacker, OutResolvedSteps, StepData, OriginCoord)) return false;

		FHexOffsetCoord DesiredCoord;
		if (!ResolveDesiredCoord(StepIntent, StepData, OriginCoord, GridManager, WorldType, DesiredCoord)) return false;

		FTargetingStep ResolvedStep;
		if (!ResolveStep(OriginCoord, DesiredCoord, StepIntent.Direction, StepData, Attacker, GridManager, WorldType, ResolvedStep)) return false;

		if (!ResolvedStep.HasDirection() && StepData.Origin.Source == ETargetingOriginSource::PreviousStep && !OutResolvedSteps.IsEmpty() && OutResolvedSteps.Last().HasDirection())
			ResolvedStep.Direction = OutResolvedSteps.Last().Direction;

		OutResolvedSteps.Add(ResolvedStep);
	}
	return true;
}


bool FBattleTargetResolver::ResolveStepOrigin(ABattleCharacterBase* Attacker, const TArray<FTargetingStep>& ResolvedSteps, const FTargetingStepCardData& StepData, FHexOffsetCoord& OutOriginCoord)
{
	switch (StepData.Origin.Source)
	{
	case ETargetingOriginSource::PreviousStep:
		if (ResolvedSteps.IsEmpty() || !ResolvedSteps.Last().HasTargetCoord()) return false;
		OutOriginCoord = ResolvedSteps.Last().TargetCoord;
		return true;
	case ETargetingOriginSource::SpecificStep:
		if (!ResolvedSteps.IsValidIndex(StepData.Origin.StepIndex) || !ResolvedSteps[StepData.Origin.StepIndex].HasTargetCoord()) return false;
		OutOriginCoord = ResolvedSteps[StepData.Origin.StepIndex].TargetCoord;
		return true;
	case ETargetingOriginSource::SourceCharacter:
	default:
		if (!IsValid(Attacker)) return false;
		OutOriginCoord = Attacker->GetCharacterCoord();
		return OutOriginCoord.IsValid();
	}
}

bool FBattleTargetResolver::ResolveDesiredCoord(const FTargetingStepIntent& StepIntent, const FTargetingStepCardData& StepData, const FHexOffsetCoord& OriginCoord, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FHexOffsetCoord& OutDesiredCoord)
{
	OutDesiredCoord = FHexOffsetCoord::Invalid();
	switch (StepData.Intent.Binding)
	{
	case ETargetingIntentBinding::TargetCharacter:
		if (ABattleCharacterBase* BoundTargetCharacter = FindCharacterByTargetingKey(GridManager, WorldType, StepIntent.TargetCharacterKey)) OutDesiredCoord = BoundTargetCharacter->GetCharacterCoord();
		else OutDesiredCoord = FHexGridMath::CubeToOffset(FHexGridMath::OffsetToCube(OriginCoord) + StepIntent.RelativeOffset);
		break;
	case ETargetingIntentBinding::WorldFixed:
		if (!StepIntent.SelectedCoord.IsValid()) return false;
		OutDesiredCoord = StepIntent.SelectedCoord;
		break;
	case ETargetingIntentBinding::OriginRelative:
	default:
		OutDesiredCoord = FHexGridMath::CubeToOffset(FHexGridMath::OffsetToCube(OriginCoord) + StepIntent.RelativeOffset);
		break;
	}
	return OutDesiredCoord.IsValid();
}

bool FBattleTargetResolver::ResolveStep(const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& DesiredCoord, int32 Direction, const FTargetingStepCardData& StepData, ABattleCharacterBase* Attacker, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FTargetingStep& OutResolvedStep)
{
	OutResolvedStep.Reset();
	if (TryResolveStepAtCoord(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager, WorldType, OutResolvedStep))
		return true;

	switch (StepData.Resolve.InvalidPolicy)
	{
	case EInvalidTargetResolvePolicy::StopAtLastValid:
		return FindLastValidStep(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager, WorldType, OutResolvedStep);
	case EInvalidTargetResolvePolicy::FindNearestValid:
		return FindNearestValidStep(OriginCoord, DesiredCoord, Direction, StepData, Attacker, GridManager, WorldType, OutResolvedStep);
	case EInvalidTargetResolvePolicy::Cancel:
	default:
		return false;
	}
}

bool FBattleTargetResolver::TryResolveStepAtCoord(const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& CandidateCoord, int32 Direction, const FTargetingStepCardData& StepData, ABattleCharacterBase* Attacker, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FTargetingStep& OutResolvedStep)
{
	OutResolvedStep.Reset();
	if (!IsValid(Attacker) || !IsValid(GridManager) || !StepData.Selection.RuleClass || !GridManager->IsValidCoord(OriginCoord) || !GridManager->IsValidCoord(CandidateCoord))
		return false;

	const UTargetSelection* Selection = StepData.Selection.RuleClass->GetDefaultObject<UTargetSelection>();
	if (!Selection || !Selection->EvaluateCandidate(GridManager, OriginCoord, CandidateCoord, StepData.Selection.RuleData, OutResolvedStep))
		return false;

	FTargetingConditionRequest Request;
	Request.SourceCharacter = Attacker;
	Request.GridManager = GridManager;
	Request.GridWorldType = WorldType;
	Request.OriginCoord = OriginCoord;
	Request.TargetCoord = CandidateCoord;
	if (!UTargetingConditionService::Evaluate(StepData.Resolve.Condition, Request))
	{
		OutResolvedStep.Reset();
		return false;
	}

	OutResolvedStep.Direction = Direction;
	if (!OutResolvedStep.HasDirection())
		OutResolvedStep.Direction = FHexGridMath::GetClosestDirection(OutResolvedStep.OriginCoord, OutResolvedStep.TargetCoord);
	return true;
}

bool FBattleTargetResolver::FindLastValidStep(const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& DesiredCoord, int32 Direction, const FTargetingStepCardData& StepData, ABattleCharacterBase* Attacker, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FTargetingStep& OutResolvedStep)
{
	OutResolvedStep.Reset();
	const int32 StepCount = FHexGridMath::GetHexDistance(OriginCoord, DesiredCoord);
	for (int32 LineStepIndex = 1; LineStepIndex <= StepCount; ++LineStepIndex)
	{
		FTargetingStep CandidateStep;
		const FHexOffsetCoord CandidateCoord = CalculateHexLineCoord(OriginCoord, DesiredCoord, LineStepIndex, StepCount);
		if (!TryResolveStepAtCoord(OriginCoord, CandidateCoord, Direction, StepData, Attacker, GridManager, WorldType, CandidateStep))
			break;
		OutResolvedStep = MoveTemp(CandidateStep);
	}
	return OutResolvedStep.HasTargetCoord();
}

bool FBattleTargetResolver::FindNearestValidStep(const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& DesiredCoord, int32 Direction, const FTargetingStepCardData& StepData, ABattleCharacterBase* Attacker, ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, FTargetingStep& OutResolvedStep)
{
	OutResolvedStep.Reset();
	if (!IsValid(GridManager)) return false;

	int32 BestDesiredDistance = MAX_int32;
	int32 BestOriginDistance = MAX_int32;
	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			FTargetingStep CandidateStep;
			const FHexOffsetCoord CandidateCoord(X, Y);
			if (!TryResolveStepAtCoord(OriginCoord, CandidateCoord, Direction, StepData, Attacker, GridManager, WorldType, CandidateStep))
				continue;

			const int32 DesiredDistance = FHexGridMath::GetHexDistance(DesiredCoord, CandidateCoord);
			const int32 OriginDistance = FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord);
			if (DesiredDistance > BestDesiredDistance || (DesiredDistance == BestDesiredDistance && OriginDistance >= BestOriginDistance))
				continue;

			BestDesiredDistance = DesiredDistance;
			BestOriginDistance = OriginDistance;
			OutResolvedStep = MoveTemp(CandidateStep);
		}
	}
	return OutResolvedStep.HasTargetCoord();
}

