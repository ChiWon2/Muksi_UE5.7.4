#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "BattleSimulationPostProcessVolume.generated.h"

class UCurveFloat;
class UMaterialInstanceDynamic;
class UMaterialInterface;

UCLASS(BlueprintType, Blueprintable)
class MUKSI_API ABattleSimulationPostProcessVolume : public APostProcessVolume
{
	GENERATED_BODY()

public:
	ABattleSimulationPostProcessVolume();
	virtual void Tick(float DeltaSeconds) override;

	// Expands the simulation post process from InTransitionOrigin.
	void ActivateSimulationPostProcess(const FVector& InTransitionOrigin);

	// Reverses the sphere transition. When complete, the runtime volume destroys itself.
	void DeactivateSimulationPostProcess();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|PostProcess")
	bool IsSimulationPostProcessActive() const;

	// Runtime configuration is injected by BattleSimulationManager.
	// A Blueprint subclass of PostProcessVolume is not required.
	void InitializeSimulationPostProcess(
		UMaterialInterface* InMaterial,
		float InBlendWeight,
		bool bInLockAutoExposure,
		float InFixedExposure,
		float InTransitionDuration,
		float InTransitionMaxRadius,
		UCurveFloat* InTransitionCurve);

private:
	void EnsureSimulationPostProcessMaterial();
	void StartTransition(bool bEntering);
	void UpdateTransitionMaterialParameters();
	void CompleteTransition();
	float EvaluateTransitionAlpha(float InAlpha) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> SimulationPostProcessMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SimulationPostProcessMID = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> TransitionCurve = nullptr;

	float SimulationBlendWeight = 1.0f;
	float TransitionDuration = 0.35f;
	float TransitionMaxRadius = 100000.0f;
	float TransitionAlpha = 0.0f;
	FVector TransitionOrigin = FVector::ZeroVector;
	bool bTransitionPlaying = false;
	bool bTransitionEntering = false;
};
