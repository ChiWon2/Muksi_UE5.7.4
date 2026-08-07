#pragma once

#include "CoreMinimal.h"
#include "BattlePhase.generated.h"

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
	None UMETA(DisplayName = "None"),
	Ready UMETA(DisplayName = "Ready"),
	ReadyEnd UMETA(DisplayName = "Ready End"),
	BattleStart UMETA(DisplayName = "Battle Start"),
	RoundStart UMETA(DisplayName = "Round Start"),
	ExchangeStart UMETA(DisplayName = "Exchange Start"),
	CardSelect UMETA(DisplayName = "Card Select"),
	Targeting UMETA(DisplayName = "Targeting"),
	CardReveal UMETA(DisplayName = "Card Reveal"),
	SimulationSequence UMETA(DisplayName = "Simulation Sequence"),
	ExchangeEnd UMETA(DisplayName = "Exchange End"),
	BattleActionSequenceStart UMETA(DisplayName = "Battle Action Sequence Start"),
	BattleActionSequenceEnd UMETA(DisplayName = "Battle Action Sequence End"),
	RoundEnd UMETA(DisplayName = "Round End"),
	BattleEnd UMETA(DisplayName = "Battle End")
};
