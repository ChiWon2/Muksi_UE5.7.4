#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "BattleSimulationTypes.generated.h"

class UMuksiBattleCardDataAsset;

/**
 * Simulation에서 각 진영의 Action 정보를 Actual / Deceived 중 어떤 것으로 해석할지 나타낸다.
 */
UENUM(BlueprintType)
enum class EBattleSimulationKnowledge : uint8
{
	Actual,
	Deceived
};

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

/**
 * WorldType 하나를 Player / Enemy 정보 정책으로 고정 변환한다.
 * 임의 조합을 저장하지 않고 Make()를 통해 4세계 규칙만 생성한다.
 */
USTRUCT(BlueprintType)
struct FBattleSimulationWorldPolicy
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyDeceived;

	UPROPERTY(BlueprintReadOnly)
	EBattleSimulationKnowledge PlayerKnowledge = EBattleSimulationKnowledge::Actual;

	UPROPERTY(BlueprintReadOnly)
	EBattleSimulationKnowledge EnemyKnowledge = EBattleSimulationKnowledge::Deceived;

	static FBattleSimulationWorldPolicy Make(EBattleSimulationWorldType InWorldType)
	{
		FBattleSimulationWorldPolicy Policy;
		Policy.WorldType = InWorldType;

		switch (InWorldType)
		{
		case EBattleSimulationWorldType::PlayerActualEnemyDeceived:
			Policy.PlayerKnowledge = EBattleSimulationKnowledge::Actual;
			Policy.EnemyKnowledge = EBattleSimulationKnowledge::Deceived;
			break;

		case EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived:
			Policy.PlayerKnowledge = EBattleSimulationKnowledge::Deceived;
			Policy.EnemyKnowledge = EBattleSimulationKnowledge::Deceived;
			break;

		case EBattleSimulationWorldType::PlayerDeceivedEnemyActual:
			Policy.PlayerKnowledge = EBattleSimulationKnowledge::Deceived;
			Policy.EnemyKnowledge = EBattleSimulationKnowledge::Actual;
			break;

		case EBattleSimulationWorldType::PlayerActualEnemyActual:
			Policy.PlayerKnowledge = EBattleSimulationKnowledge::Actual;
			Policy.EnemyKnowledge = EBattleSimulationKnowledge::Actual;
			break;

		default:
			checkNoEntry();
			break;
		}

		return Policy;
	}

	EBattleSimulationKnowledge GetKnowledge(bool bPlayerAction) const
	{
		return bPlayerAction ? PlayerKnowledge : EnemyKnowledge;
	}

	bool UsesSimulationRuntime() const
	{
		return WorldType != EBattleSimulationWorldType::PlayerActualEnemyActual;
	}
};

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
