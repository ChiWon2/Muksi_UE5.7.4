#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "BattleSimulationPostProcessVolume.generated.h"

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	FVector4 GrayscaleSaturation = FVector4(0.0, 0.0, 0.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	FVector4 GrayscaleContrast = FVector4(1.35, 1.35, 1.35, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SimulationBlendWeight = 1.0f;
};