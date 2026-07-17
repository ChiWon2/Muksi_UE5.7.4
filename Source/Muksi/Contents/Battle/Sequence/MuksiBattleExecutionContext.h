#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

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

	FMuksiRequestSystemExecution RequestSystemExecution;

	bool IsValidContext() const
	{
		return Attacker != nullptr && Card != nullptr;
	}

	FIntPoint GetMainTargetPoint() const
	{
		return TargetPoints.IsValidIndex(0) ? TargetPoints[0] : FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	bool CanRequestSystemExecution() const
	{
		return RequestSystemExecution.IsBound();
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
