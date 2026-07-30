#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"

class ABattleCharacterBase;
class ABattleGridManager;
struct FTargetingStepCardData;

class MUKSI_API FBattleTargetResolver
{
public:
	static FTargetingIntent CaptureIntent(const FTargetingResult& TargetingResult, const FHexOffsetCoord& SourceCoord);
	static bool ResolveAction(FBattleAction& InOutAction, ABattleGridManager* GridManager);

private:
	static bool ResolveStepOrigin(
		const FBattleAction& Action,
		const FTargetingResult& ResolvedResult,
		const FTargetingStepCardData* StepData,
		FHexOffsetCoord& OutOriginCoord
	);

	static bool ResolveStepSelectedCoord(
		const FBattleAction& Action,
		const FTargetingResult& SourceResult,
		const FTargetingStepIntent& StepIntent,
		const FTargetingStepCardData* StepData,
		int32 StepIndex,
		const FHexOffsetCoord& OriginCoord,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutSelectedCoord,
		TArray<TObjectPtr<ABattleCharacterBase>>& OutTargetCharacters
	);

	static bool ResolveInvalidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		const FTargetingStepCardData* StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static bool IsCoordUsable(
		const FHexOffsetCoord& Coord,
		const FTargetingStepCardData* StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager
	);

	static bool FindLastValidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		const FTargetingStepCardData* StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static bool FindNearestValidCoord(
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& DesiredCoord,
		const FTargetingStepCardData* StepData,
		ABattleCharacterBase* Attacker,
		ABattleGridManager* GridManager,
		FHexOffsetCoord& OutResolvedCoord
	);

	static FHexOffsetCoord ResolveCubeLineCoord(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& EndCoord, int32 StepIndex, int32 StepCount);
	static bool ApplyFinalPattern(FBattleAction& InOutAction, ABattleGridManager* GridManager);
};
