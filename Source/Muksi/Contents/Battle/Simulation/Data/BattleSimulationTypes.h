#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "BattleSimulationTypes.generated.h"

class UMuksiBattleCardDataAsset;

UENUM(BlueprintType)
enum class EBattleSimulationState : uint8
{
	Idle,
	Starting,
	WaitingForPlayerTargeting,
	RevealingEnemyCard,
	ResolvingActionOrder,
	ExecutingFirstAction,
	ExecutingSecondAction,
	FinishingExchange,
	FinishingSimulation,
	Completed
};

USTRUCT(BlueprintType)
struct FBattleSimulationActionPlan
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FBattleAction SequenceAction;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SimulationCardOverride = nullptr;

	void Set(const FBattleAction& InSequenceAction, UMuksiBattleCardDataAsset* InSimulationCardOverride)
	{
		SequenceAction = InSequenceAction;
		SimulationCardOverride = InSimulationCardOverride;
	}

	UMuksiBattleCardDataAsset* GetSimulationCard() const
	{
		return SimulationCardOverride ? SimulationCardOverride.Get() : SequenceAction.Card.Get();
	}

	void Reset()
	{
		SequenceAction = FBattleAction();
		SimulationCardOverride = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FBattleSimulationExchange
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	FBattleSimulationActionPlan PlayerAction;

	UPROPERTY(BlueprintReadOnly)
	FBattleSimulationActionPlan EnemyAction;

	UPROPERTY(BlueprintReadOnly)
	bool bHasPlayerAction = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasEnemyAction = false;

	UPROPERTY(BlueprintReadOnly)
	bool bPlayerFirst = false;

	UPROPERTY(BlueprintReadOnly)
	bool bActionOrderResolved = false;

	void Reset(int32 InExchangeIndex)
	{
		ExchangeIndex = InExchangeIndex;
		PlayerAction.Reset();
		EnemyAction.Reset();
		bHasPlayerAction = false;
		bHasEnemyAction = false;
		bPlayerFirst = false;
		bActionOrderResolved = false;
	}

	void SetPlayerAction(const FBattleAction& InAction, UMuksiBattleCardDataAsset* SimulationCardOverride)
	{
		PlayerAction.Set(InAction, SimulationCardOverride);
		PlayerAction.SequenceAction.ExchangeIndex = ExchangeIndex;
		PlayerAction.SequenceAction.bPlayerAction = true;
		bHasPlayerAction = true;
		bActionOrderResolved = false;
	}

	void SetEnemyAction(const FBattleAction& InAction, UMuksiBattleCardDataAsset* SimulationCardOverride)
	{
		EnemyAction.Set(InAction, SimulationCardOverride);
		EnemyAction.SequenceAction.ExchangeIndex = ExchangeIndex;
		EnemyAction.SequenceAction.bPlayerAction = false;
		bHasEnemyAction = true;
		bActionOrderResolved = false;
	}

	bool CanResolveActionOrder() const
	{
		return bHasPlayerAction && bHasEnemyAction;
	}

	bool ResolveActionOrder()
	{
		if (!CanResolveActionOrder())
		{
			return false;
		}

		bPlayerFirst = PlayerAction.SequenceAction.Speed >= EnemyAction.SequenceAction.Speed;
		bActionOrderResolved = true;
		return true;
	}

	const FBattleSimulationActionPlan& GetFirstAction() const
	{
		check(bActionOrderResolved);
		return bPlayerFirst ? PlayerAction : EnemyAction;
	}

	const FBattleSimulationActionPlan& GetSecondAction() const
	{
		check(bActionOrderResolved);
		return bPlayerFirst ? EnemyAction : PlayerAction;
	}
};
