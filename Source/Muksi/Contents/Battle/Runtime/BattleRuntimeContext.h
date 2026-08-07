#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "UObject/Object.h"
#include "BattleRuntimeContext.generated.h"

class ABattleCharacterBase;
class ABattleCharacter_Player;
class ABattleCharacter_Enemy;

/**
 * 전투 Manager들이 공유하는 현재 전투 세션 데이터다.
 * 전투 흐름을 진행하지 않으며, 각 Manager가 생성한 결과만 보관한다.
 */
UCLASS(BlueprintType)
class MUKSI_API UBattleRuntimeContext : public UObject
{
	GENERATED_BODY()

public:
	void ResetBattle();
	void ResetRound(int32 InRound);

	void SetCurrentExchange(int32 InExchange) { CurrentExchange = InExchange; }
	void SetBattleCharacters(ABattleCharacter_Player* InPlayerCharacter, ABattleCharacter_Enemy* InEnemyCharacter);

	int32 GetCurrentRound() const { return CurrentRound; }
	int32 GetCurrentExchange() const { return CurrentExchange; }
	ABattleCharacter_Player* GetPlayerCharacter() const { return PlayerCharacter; }
	ABattleCharacter_Enemy* GetEnemyCharacter() const { return EnemyCharacter; }

	void SetRuntimeCharacterOverride(ABattleCharacterBase* SourceCharacter, ABattleCharacterBase* RuntimeCharacter);
	void ClearRuntimeCharacterOverrides();
	ABattleCharacterBase* ResolveRuntimeCharacter(const ABattleCharacterBase* SourceCharacter) const;

	void ResetExchangeActions(int32 ExchangeIndex);
	void SetPlayerExchangeAction(int32 ExchangeIndex, const FBattleAction& Action);
	void SetEnemyExchangeAction(int32 ExchangeIndex, const FBattleAction& Action);
	void ClearPlayerExchangeAction(int32 ExchangeIndex);
	void ClearEnemyExchangeAction(int32 ExchangeIndex);
	const FBattleAction* GetPlayerExchangeAction(int32 ExchangeIndex) const;
	const FBattleAction* GetEnemyExchangeAction(int32 ExchangeIndex) const;
	bool HasExchangeActions(int32 ExchangeIndex) const;

	void ClearBattleActionSequenceQueue();
	void AppendBattleActionSequenceAction(const FBattleAction& Action);
	const TArray<FBattleAction>& GetBattleActionSequenceQueue() const { return BattleActionSequenceQueue; }
	bool HasBattleActionSequenceActions() const { return !BattleActionSequenceQueue.IsEmpty(); }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Runtime", meta = (AllowPrivateAccess = "true"))
	int32 CurrentRound = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Runtime", meta = (AllowPrivateAccess = "true"))
	int32 CurrentExchange = 0;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Runtime", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABattleCharacter_Player> PlayerCharacter = nullptr;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Runtime", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABattleCharacter_Enemy> EnemyCharacter = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleCharacterBase>> RuntimeCharacterOverrides;

	UPROPERTY(Transient)
	TArray<FBattleAction> PlayerExchangeActions;

	UPROPERTY(Transient)
	TArray<FBattleAction> EnemyExchangeActions;

	UPROPERTY(Transient)
	TArray<FBattleAction> BattleActionSequenceQueue;
};
