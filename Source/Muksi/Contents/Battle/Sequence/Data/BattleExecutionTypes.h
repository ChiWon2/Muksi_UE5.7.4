#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "BattleExecutionTypes.generated.h"

class UBattleExecution;

USTRUCT(BlueprintType)
struct FBattleExecutionData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FBattleExecutionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution")
	TSubclassOf<UBattleExecution> ExecutionClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution", meta = (BaseStruct = "/Script/Muksi.BattleExecutionData"))
	FInstancedStruct ExecutionData;

	bool IsValid() const;
	void SyncExecutionDataType();
};

USTRUCT(BlueprintType)
struct FBattleNotifyExecutionChain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	TArray<FBattleExecutionEntry> Executions;

	bool IsValid() const;
	void SyncExecutionDataTypes();
};