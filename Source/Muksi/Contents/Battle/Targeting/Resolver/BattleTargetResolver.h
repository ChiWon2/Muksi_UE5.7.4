#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"

class ABattleCharacterBase;
class ABattleGridManager;
struct FTargetingCardData;
struct FResolvedTargeting;
struct FTargetingStepCardData;
struct FTargetingStepResult;

class MUKSI_API FBattleTargetResolver
{
public:
	static bool ResolveAction(const FBattleAction& Action, ABattleGridManager* GridManager, FResolvedTargeting& OutResolvedTargeting, const FTargetingCardData* TargetingDataOverride = nullptr);
	static bool ResolveActionThroughStep(const FBattleAction& Action, ABattleGridManager* GridManager, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting, const FTargetingCardData* TargetingDataOverride = nullptr);
	static bool ResolveIntent(
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		const FTargetingCardData& TargetingData,
		const FTargetingIntent& TargetingIntent,
		FResolvedTargeting& OutResolvedTargeting
	);

private:
	static bool ResolveStepOrigin(
		ABattleCharacterBase* Attacker,
		const FResolvedTargeting& ResolvedTargeting,
		const FTargetingStepCardData& StepData,
		FHexOffsetCoord& OutOriginCoord
	);

	static bool ResolveDesiredCoord(
		const FTargetingStepIntent& StepIntent,
		const FTargetingStepCardData& StepData,
		const FHexOffsetCoord& OriginCoord,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutDesiredCoord
	);

	static bool ResolveInvalidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static bool EvaluateStepAtCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& CandidateCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FTargetingStepResult& OutStepResult
	);

	static bool IsCoordUsable(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& Coord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager
	);

	static bool FindLastValidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static bool FindNearestValidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		int32 Direction,
		const FTargetingStepCardData& StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static FHexOffsetCoord ResolveCubeLineCoord(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& EndCoord, int32 LinePointIndex, int32 LinePointCount);
	static bool ApplyFinalPattern(
		ABattleGridManager* GridManager,
		const FTargetingCardData& TargetingData,
		FResolvedTargeting& InOutResolvedTargeting
	);
};
