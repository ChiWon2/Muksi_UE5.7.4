#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "BattleSimulationTypes.generated.h"

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
struct FBattleSimulationExchange
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	FBattleAction PlayerAction;

	UPROPERTY(BlueprintReadOnly)
	FBattleAction EnemyAction;

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
		PlayerAction = FBattleAction();
		EnemyAction = FBattleAction();
		bHasPlayerAction = false;
		bHasEnemyAction = false;
		bPlayerFirst = false;
		bActionOrderResolved = false;
	}

	void SetPlayerAction(const FBattleAction& InAction)
	{
		PlayerAction = InAction;
		PlayerAction.ExchangeIndex = ExchangeIndex;
		PlayerAction.bPlayerAction = true;
		bHasPlayerAction = true;
		bActionOrderResolved = false;
	}

	void SetEnemyAction(const FBattleAction& InAction)
	{
		EnemyAction = InAction;
		EnemyAction.ExchangeIndex = ExchangeIndex;
		EnemyAction.bPlayerAction = false;
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

		bPlayerFirst = PlayerAction.Speed >= EnemyAction.Speed;
		bActionOrderResolved = true;
		return true;
	}

	const FBattleAction& GetFirstAction() const
	{
		check(bActionOrderResolved);
		return bPlayerFirst ? PlayerAction : EnemyAction;
	}

	const FBattleAction& GetSecondAction() const
	{
		check(bActionOrderResolved);
		return bPlayerFirst ? EnemyAction : PlayerAction;
	}
};