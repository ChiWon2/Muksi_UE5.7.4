#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "UObject/Object.h"
#include "BattleSimulationPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleCharacter_Enemy;
class ABattleSimulationManager;
class ABattleSimulationPostProcessVolume;
class ABattleSimulationCharacter;
class ABattleDDPresentationActor;
class UBattleSimulationWorldRuntime;
class UTargetingPresentationController;
struct FTargetingStepCardData;

USTRUCT()
struct FBattleSimulationPreviewData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FBattleAction Action;

	UPROPERTY(Transient)
	FTargetingResult TargetingResult;

};

UCLASS(BlueprintType)
class MUKSI_API UBattleSimulationPresentationController : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager, UTargetingPresentationController* InTargetingPresentationController);
	void Shutdown();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Time")
	float GetSimulationTimeScale() const;

	UBattleSimulationWorldRuntime* GetPrimaryPresentationWorldRuntime() const;

	bool EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void ExitSimulationPresentation(bool bClearPreviewData);
	void ClearAllPreviewData();
	void UpdatePreviewData(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void RemovePreviewData(UBattleSimulationWorldRuntime* WorldRuntime);
	// ExchangeEnd에서 AD/DD 적 위치를 한 번 비교해 DD 위치 표기를 갱신한다.
	// 표시 여부는 위치 차이만 사용하며 회전/애니메이션 차이는 무시한다.
	void RefreshDDPresentationAtExchangeEnd();
	// BattleActionSequence에 들어가기 직전 DD 위치 표기를 숨긴다. Actor는 BattleEnd까지 재사용한다.
	void HideDDPresentation();
	bool PrewarmDDPresentationActor(ABattleCharacter_Enemy* SourceEnemy);

private:
	void SynchronizeSimulationPresentation();
	void DestroyDDPresentationActor();
	void DisplayExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPreviewData& PreviewData);
	void ClearExecutionPreview();
	void AddExecutionStepPreview(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FTargetingResult& TargetingResult);
	bool CreateSimulationPostProcess(const FVector& TransitionOrigin);
	FVector ResolveSimulationPostProcessTransitionOrigin(const TArray<ABattleCharacterBase*>& SourceCharacters) const;
	void DestroySimulationPostProcess();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<EBattleSimulationWorldType, FBattleSimulationPreviewData> PreviewDataByWorld;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	// The battle has exactly one enemy. Spawn this once during BattleStart and reuse it for every round/simulation.
	UPROPERTY(Transient)
	TObjectPtr<ABattleDDPresentationActor> DDPresentationActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacter_Enemy> DDPresentationSourceEnemy = nullptr;

	bool bSimulationPresentationActive = false;
};
