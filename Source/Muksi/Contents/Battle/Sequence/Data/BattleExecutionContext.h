#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "BattleExecutionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;

struct FBattleExecutionContext;

DECLARE_DELEGATE_TwoParams(FRequestRuntimeExecutionChain, const TArray<FBattleExecutionEntry>&, const FBattleExecutionContext&);

USTRUCT(BlueprintType)
struct FBattleExecutionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;


	UPROPERTY(BlueprintReadOnly)
	FTargetingResult TargetingResult;

	UPROPERTY(BlueprintReadOnly)
	FInstancedStruct ExecutionData;


	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FName NotifyKey = NAME_None;

	//특정 Runtime Execution이 현재 처리할 단일 캐릭터 -- HitReaction등에서 사용된다. Attacker와는 다른 ExecutionTarget이다.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> ExecutionTarget = nullptr;

	//런타임 결과에 따라 동적으로 발생하는 Execution을 Manager에 전달하는 통로이다.
	//HitReaction같은 경우, 데미지 정보 등을 담고있는 Execution에 반응하는 Execution이기 때문에 이렇게 동작시킨다.
	FRequestRuntimeExecutionChain RequestRuntimeExecutionChain;

	bool IsValidContext() const
	{
		return Attacker != nullptr && Card != nullptr;
	}

	FIntPoint GetMainTargetPoint() const
	{
		if (TargetingResult.HasSelectedCoord())
		{
			return TargetingResult.GetSelectedCoord();
		}

		return TargetingResult.AffectedCoords.IsValidIndex(0) ? TargetingResult.AffectedCoords[0] : FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	bool CanRequestRuntimeExecutionChain() const
	{
		return RequestRuntimeExecutionChain.IsBound();
	}

	bool HasExecutionData() const
	{
		return ExecutionData.IsValid();
	}

	template<typename T>
	const T* GetExecutionData() const
	{
		return ExecutionData.GetPtr<T>();
	}

	template<typename T>
	T* GetMutableExecutionData()
	{
		return ExecutionData.GetMutablePtr<T>();
	}

	template<typename T>
	bool IsExecutionDataType() const
	{
		return ExecutionData.GetScriptStruct() == T::StaticStruct();
	}
};