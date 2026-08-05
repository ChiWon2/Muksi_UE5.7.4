#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"

#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"

void UBattleRuntimeContext::ResetBattle()
{
	CurrentRound = 0;
	CurrentExchange = 0;
	PlayerCharacter = nullptr;
	EnemyCharacter = nullptr;
	RuntimeCharacterOverrides.Empty();
	PlayerExchangeActions.Empty();
	EnemyExchangeActions.Empty();
	BattleActionSequenceQueue.Empty();
}

void UBattleRuntimeContext::ResetRound(const int32 InRound)
{
	CurrentRound = InRound;
	CurrentExchange = 0;
	RuntimeCharacterOverrides.Empty();
	PlayerExchangeActions.Empty();
	EnemyExchangeActions.Empty();
	BattleActionSequenceQueue.Empty();
}

void UBattleRuntimeContext::SetBattleCharacters(
	ABattleCharacter_Player* InPlayerCharacter,
	ABattleCharacter_Enemy* InEnemyCharacter)
{
	PlayerCharacter = InPlayerCharacter;
	EnemyCharacter = InEnemyCharacter;
}

void UBattleRuntimeContext::SetRuntimeCharacterOverride(
	ABattleCharacterBase* SourceCharacter,
	ABattleCharacterBase* RuntimeCharacter)
{
	if (!IsValid(SourceCharacter))
	{
		return;
	}

	if (!IsValid(RuntimeCharacter) || RuntimeCharacter == SourceCharacter)
	{
		RuntimeCharacterOverrides.Remove(SourceCharacter);
		return;
	}

	RuntimeCharacterOverrides.Add(SourceCharacter, RuntimeCharacter);
}

void UBattleRuntimeContext::ClearRuntimeCharacterOverrides()
{
	RuntimeCharacterOverrides.Empty();
}

ABattleCharacterBase* UBattleRuntimeContext::ResolveRuntimeCharacter(
	const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter))
	{
		return nullptr;
	}

	const TObjectPtr<ABattleCharacterBase>* RuntimeCharacter =
		RuntimeCharacterOverrides.Find(const_cast<ABattleCharacterBase*>(SourceCharacter));
	return RuntimeCharacter && IsValid(RuntimeCharacter->Get())
		? RuntimeCharacter->Get()
		: const_cast<ABattleCharacterBase*>(SourceCharacter);
}

void UBattleRuntimeContext::ResetExchangeActions(const int32 ExchangeIndex)
{
	if (ExchangeIndex < 0)
	{
		return;
	}

	if (PlayerExchangeActions.Num() > ExchangeIndex)
	{
		PlayerExchangeActions.SetNum(ExchangeIndex);
	}

	if (EnemyExchangeActions.Num() > ExchangeIndex)
	{
		EnemyExchangeActions.SetNum(ExchangeIndex);
	}
}

void UBattleRuntimeContext::SetPlayerExchangeAction(const int32 ExchangeIndex, const FBattleAction& Action)
{
	if (ExchangeIndex < 0)
	{
		return;
	}

	PlayerExchangeActions.SetNum(FMath::Max(PlayerExchangeActions.Num(), ExchangeIndex + 1));
	PlayerExchangeActions[ExchangeIndex] = Action;
}

void UBattleRuntimeContext::SetEnemyExchangeAction(const int32 ExchangeIndex, const FBattleAction& Action)
{
	if (ExchangeIndex < 0)
	{
		return;
	}

	EnemyExchangeActions.SetNum(FMath::Max(EnemyExchangeActions.Num(), ExchangeIndex + 1));
	EnemyExchangeActions[ExchangeIndex] = Action;
}

void UBattleRuntimeContext::ClearPlayerExchangeAction(const int32 ExchangeIndex)
{
	if (ExchangeIndex >= 0 && PlayerExchangeActions.Num() > ExchangeIndex)
	{
		PlayerExchangeActions.SetNum(ExchangeIndex);
	}
}

void UBattleRuntimeContext::ClearEnemyExchangeAction(const int32 ExchangeIndex)
{
	if (ExchangeIndex >= 0 && EnemyExchangeActions.Num() > ExchangeIndex)
	{
		EnemyExchangeActions.SetNum(ExchangeIndex);
	}
}

const FBattleAction* UBattleRuntimeContext::GetPlayerExchangeAction(const int32 ExchangeIndex) const
{
	return PlayerExchangeActions.IsValidIndex(ExchangeIndex)
		? &PlayerExchangeActions[ExchangeIndex]
		: nullptr;
}

const FBattleAction* UBattleRuntimeContext::GetEnemyExchangeAction(const int32 ExchangeIndex) const
{
	return EnemyExchangeActions.IsValidIndex(ExchangeIndex)
		? &EnemyExchangeActions[ExchangeIndex]
		: nullptr;
}

bool UBattleRuntimeContext::HasExchangeActions(const int32 ExchangeIndex) const
{
	const FBattleAction* PlayerAction = GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = GetEnemyExchangeAction(ExchangeIndex);
	return PlayerAction && EnemyAction
		&& IsValid(PlayerAction->Attacker) && IsValid(PlayerAction->Card)
		&& IsValid(EnemyAction->Attacker) && IsValid(EnemyAction->Card);
}

void UBattleRuntimeContext::ClearBattleActionSequenceQueue()
{
	BattleActionSequenceQueue.Empty();
}

void UBattleRuntimeContext::AppendBattleActionSequenceAction(const FBattleAction& Action)
{
	BattleActionSequenceQueue.Add(Action);
}
