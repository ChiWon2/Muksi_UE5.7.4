#pragma once

#include "CoreMinimal.h"
#include "BattleSimulationTypes.generated.h"

/**
 * 전투 정보 4세계 모델. AA는 실제 Battle World가 담당하고 AD / DD / DA만 Simulation Runtime 대상이다.
 */
UENUM(BlueprintType)
enum class EBattleSimulationWorldType : uint8
{
	PlayerActualEnemyDeceived UMETA(DisplayName = "AD - Player Actual / Enemy Deceived"),
	PlayerDeceivedEnemyDeceived UMETA(DisplayName = "DD - Player Deceived / Enemy Deceived"),
	PlayerDeceivedEnemyActual UMETA(DisplayName = "DA - Player Deceived / Enemy Actual"),
	PlayerActualEnemyActual UMETA(DisplayName = "AA - Player Actual / Enemy Actual")
};

/**
 * Player UI가 접근 가능한 Simulation View만 노출한다. ActualSelf는 AD, DeceivedSelf는 DD에 대응한다.
 */
UENUM(BlueprintType)
enum class EBattlePlayerSimulationView : uint8
{
	ActualSelf UMETA(DisplayName = "Actual Self - AD"),
	DeceivedSelf UMETA(DisplayName = "Deceived Self - DD")
};

namespace BattleSimulationWorld
{
	inline bool UsesActualCard(EBattleSimulationWorldType WorldType, bool bPlayerAction)
	{
		if (bPlayerAction) return WorldType == EBattleSimulationWorldType::PlayerActualEnemyDeceived || WorldType == EBattleSimulationWorldType::PlayerActualEnemyActual;
		return WorldType == EBattleSimulationWorldType::PlayerDeceivedEnemyActual || WorldType == EBattleSimulationWorldType::PlayerActualEnemyActual;
	}

	inline bool UsesSimulationRuntime(EBattleSimulationWorldType WorldType)
	{
		return WorldType != EBattleSimulationWorldType::PlayerActualEnemyActual;
	}
}

UENUM(BlueprintType)
enum class EBattleSimulationState : uint8
{
	Idle,
	Ready,
	Prepared,
	ExecutingFirstAction,
	ExecutingSecondAction,
	Completed
};
