#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "BattleSimulationPostProcessVolume.generated.h"

class UMaterialInterface;

UCLASS(BlueprintType, Blueprintable)
class MUKSI_API ABattleSimulationPostProcessVolume : public APostProcessVolume
{
	GENERATED_BODY()

public:
	ABattleSimulationPostProcessVolume();

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|PostProcess")
	void ActivateSimulationPostProcess();

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|PostProcess")
	void DeactivateSimulationPostProcess();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|PostProcess")
	bool IsSimulationPostProcessActive() const;

	// Runtime configuration is injected by BattleSimulationManager.
	// A Blueprint subclass of PostProcessVolume is not required.
	void InitializeSimulationPostProcess(UMaterialInterface* InMaterial, float InBlendWeight);

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> SimulationPostProcessMaterial = nullptr;

	float SimulationBlendWeight = 1.0f;

	void EnsureSimulationPostProcessMaterial();
};