#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleExecutionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;
class UMuksiBattleExecution;

struct FMuksiBattleExecutionContext;

DECLARE_DELEGATE_TwoParams(FMuksiRequestSystemExecution, TSubclassOf<UMuksiBattleExecution>, const FMuksiBattleExecutionContext&);

USTRUCT(BlueprintType)
struct FMuksiBattleExecutionContext
{
	GENERATED_BODY()

	/**
	 * 현재 행동을 수행하는 캐릭터
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;

	/**
	 * 현재 Execution이 직접 대상으로 삼는 캐릭터
	 *
	 * 일반적인 MainEffect에서는 비어 있을 수 있으며,
	 * HitReaction 같은 대상 전용 Execution을 실행할 때 설정한다.
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

	/**
	 * 현재 행동에서 사용한 카드
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	/**
	 * 전투 Grid Manager
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	/**
	 * 카드 행동의 대상 좌표
	 *
	 * Index 0은 MainTargetPoint로 사용한다.
	 */
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> TargetPoints;

	/**
	 * 현재 Execution을 실행시킨 Animation Notify Key
	 */
	UPROPERTY(BlueprintReadOnly)
	FName NotifyKey = NAME_None;

	/**
	 * Execution 내부에서 새로운 System Execution을 요청할 때 사용한다.
	 *
	 * 실제 Execution 생성과 PendingExecutionCount 관리는
	 * BattleSequenceManager가 담당한다.
	 *
	 * UPROPERTY 대상이 아닌 일반 Delegate이다.
	 */
	FMuksiRequestSystemExecution RequestSystemExecution;

public:
	bool IsValidContext() const
	{
		return Attacker.Get() != nullptr && Card.Get() != nullptr;
	}

	FIntPoint GetMainTargetPoint() const
	{
		return TargetPoints.IsValidIndex(0)? TargetPoints[0] : FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	bool CanRequestSystemExecution() const
	{
		return RequestSystemExecution.IsBound();
	}
};