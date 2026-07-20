#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
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
	TObjectPtr<ABattleCharacterBase> ExecutionTarget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> TargetPoints;

	UPROPERTY(BlueprintReadOnly)
	FName NotifyKey = NAME_None;

	UPROPERTY(BlueprintReadOnly)
	FInstancedStruct ExecutionData;

	FRequestRuntimeExecutionChain RequestRuntimeExecutionChain;

	bool IsValidContext() const
	{
		return Attacker != nullptr && Card != nullptr;
	}

	FIntPoint GetMainTargetPoint() const
	{
		return TargetPoints.IsValidIndex(0) ? TargetPoints[0] : FIntPoint(INDEX_NONE, INDEX_NONE);
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