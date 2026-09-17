#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "BattleExecutionTypes.generated.h"

class ABattleCharacterBase;
class UBattleExecution;

UENUM(BlueprintType)
enum class EBattleExecutionMode : uint8
{
	Simulation,
	ActualBattle
};

UENUM(BlueprintType)
enum class EBattleExecutionScope : uint8
{
	Both,
	SimulationOnly,
	ActualBattleOnly
};

UENUM(BlueprintType)
enum class EBattleExecutionTargetPolicy : uint8
{
	TargetingResult,
	ExecutionTarget,
	Attacker
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution")
	EBattleExecutionScope ExecutionScope = EBattleExecutionScope::Both;

	// 런타임에서 이 Entry를 실행할 캐릭터를 교체한다. 비어 있으면 Context의 Attacker를 그대로 사용한다.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Execution|Runtime")
	TObjectPtr<ABattleCharacterBase> ExecutionSourceOverride = nullptr;

	// 런타임에서 이 Entry가 직접 참조할 대상을 지정한다. 비어 있으면 기존 ExecutionTarget을 그대로 사용한다.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Execution|Runtime")
	TObjectPtr<ABattleCharacterBase> ExecutionTargetOverride = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution", meta = (BaseStruct = "/Script/Muksi.BattleExecutionData"))
	FInstancedStruct ExecutionData;

	bool IsValid() const;
	void SyncExecutionDataType();
};

USTRUCT(BlueprintType)
struct FBattleExecutionNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	TArray<FBattleExecutionEntry> ExecutionEntries;

	// 비어 있으면 BattleAction의 Attacker가 Notify Source다. 런타임 Action 편집 시에만 Override한다.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Notify|Runtime")
	TObjectPtr<ABattleCharacterBase> NotifySourceOverride = nullptr;

	bool IsValid() const;
	void SyncExecutionDataTypes();
};
