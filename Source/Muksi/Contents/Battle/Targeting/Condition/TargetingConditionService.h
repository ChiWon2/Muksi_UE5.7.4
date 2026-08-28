#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingCondNode.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Context/TargetingConditionContext.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "TargetingConditionService.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

USTRUCT()
struct FTargetingConditionRequest
{
    GENERATED_BODY()

    TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;
    TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;
    TObjectPtr<ABattleGridManager> GridManager = nullptr;
    EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
    FHexOffsetCoord OriginCoord;
    FHexOffsetCoord TargetCoord;
};

/** Resolve Condition을 평가하는 진입점. */
UCLASS()
class MUKSI_API UTargetingConditionService : public UObject
{
    GENERATED_BODY()

public:
    static bool Evaluate(const FTargetingCondNode& Conditions, const FTargetingConditionRequest& Request);
};
