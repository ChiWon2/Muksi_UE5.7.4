#pragma once

#include "CoreMinimal.h"
#include "BattlePhase.generated.h"

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
	None UMETA(DisplayName = "None"),
	Ready UMETA(DisplayName = "Ready"),
	ReadyEnd UMETA(DisplayName = "ReadyEnd"),
	BattleStart UMETA(DisplayName = "Battle Start"),
	RoundStart UMETA(DisplayName = "Round Start"),
	ExchangeStart UMETA(DisplayName = "Exchange Start"),
	ExchangeEnd UMETA(DisplayName = "Exchange End"),
	AttackStart UMETA(DisplayName = "Attack Start"),
	AttackEnd UMETA(DisplayName = "Attack End"),
	RoundEnd UMETA(DisplayName = "Round End"),
	BattleEnd UMETA(DisplayName = "Battle End")
};

UENUM(BlueprintType)
enum class EBattleExchangePhase : uint8
{
	Idle,
	CardSelecting,
	Targeting,
	CardRevealing,
	Simulating
};