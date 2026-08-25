#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleExecutionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleExecutionEnvironment;
class UMuksiBattleCardDataAsset;

struct FBattleExecutionContext;

DECLARE_DELEGATE_RetVal_ThreeParams(bool, FRequestRuntimeExecutionChain, const TArray<FBattleExecutionEntry>&, const FBattleExecutionContext&, FSimpleDelegate);

USTRUCT(BlueprintType)
struct FBattleExecutionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EBattleExecutionMode ExecutionMode = EBattleExecutionMode::Sequence;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBattleExecutionEnvironment> Environment = nullptr;

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
	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(BlueprintReadOnly)
	FName NotifyKey = NAME_None;

	//특정 Runtime Execution이 현재 처리할 단일 캐릭터 -- HitReaction등에서 사용된다. Attacker와는 다른 ExecutionTarget이다.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> ExecutionTarget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 NestedChainDepth = 0;

	//런타임 결과에 따라 동적으로 발생하는 자식 Chain을 현재 Runner에 전달하는 통로이다.
	FRequestRuntimeExecutionChain RequestRuntimeExecutionChain;

	bool IsValidContext() const
	{
		return Attacker != nullptr;
	}

	bool HasValidEnvironment() const;

	const FTargetingStepResult* GetLastTargetingStepResult() const
	{
		return TargetingResult.GetLastStep();
	}

	FHexOffsetCoord GetPrimaryTargetCoord() const
	{
		const FTargetingStepResult* StepResult = GetLastTargetingStepResult();
		if (!StepResult) return FHexOffsetCoord::Invalid();
		if (StepResult->ResolvedStep.HasResolvedCoord()) return StepResult->ResolvedStep.ResolvedCoord;
		return StepResult->AffectedCoords.IsValidIndex(0) ? StepResult->AffectedCoords[0] : FHexOffsetCoord::Invalid();
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
