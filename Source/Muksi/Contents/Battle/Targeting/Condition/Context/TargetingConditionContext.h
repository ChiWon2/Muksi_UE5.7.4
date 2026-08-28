#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "TargetingConditionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

/** ConditionHandleSystem에 타겟팅 평가 정보를 전달하는 transient context. */
UCLASS()
class MUKSI_API UTargetingConditionContext : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleGridManager> GridManager = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

    UPROPERTY(Transient, BlueprintReadOnly)
    FHexOffsetCoord OriginCoord;

    UPROPERTY(Transient, BlueprintReadOnly)
    FHexOffsetCoord TargetCoord;
};
