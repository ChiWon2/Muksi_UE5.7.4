#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleSequenceDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FMuksiBattleExecutionData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FMuksiBattleExecutionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution")
	TSubclassOf<UMuksiBattleExecution> ExecutionClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution", meta = (BaseStruct = "/Script/Muksi.MuksiBattleExecutionData"))
	FInstancedStruct ExecutionData;

	bool IsValid() const
	{
		return ExecutionClass != nullptr;
	}

	void SyncExecutionDataType();
};

USTRUCT(BlueprintType)
struct FMuksiBattleNotifyExecutionBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	TArray<FMuksiBattleExecutionEntry> ExecutionChain;

	bool IsValid() const
	{
		return !NotifyKey.IsNone() && !ExecutionChain.IsEmpty();
	}

	void SyncExecutionDataTypes();
};