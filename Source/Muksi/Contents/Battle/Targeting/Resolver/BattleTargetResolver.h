#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"

class ABattleCharacterBase;
class ABattleGridManager;
struct FTargetingCardData;
struct FTargetingStepCardData;

/** StepIntent를 현재 Simulation World 기준 Coord / Direction으로 해석한다. Pattern 계산은 담당하지 않는다. */
class MUKSI_API FBattleTargetResolver
{
public:
	static bool ResolveIntent(
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		const FTargetingCardData& TargetingData,
		const FTargetingIntent& TargetingIntent,
		TArray<FTargetingStep>& OutResolvedSteps);

private:
	static bool ResolveStepOrigin(
		ABattleCharacterBase* Attacker,
		const TArray<FTargetingStep>& ResolvedSteps,
		const FTargetingStepCardData& StepData,
		FHexOffsetCoord& OutOriginCoord);

	static bool ResolveDesiredCoord(
		const FTargetingStepIntent& StepIntent,
		const FTargetingStepCardData& StepData,
		const FHexOffsetCoord& OriginCoord,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		FHexOffsetCoord& OutDesiredCoord);

	static bool ResolveStep(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		FTargetingStep& OutResolvedStep);

	static bool TryResolveStepAtCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& CandidateCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		FTargetingStep& OutResolvedStep);

	static bool FindLastValidStep(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		FTargetingStep& OutResolvedStep);

	static bool FindNearestValidStep(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		FTargetingStep& OutResolvedStep);
};
