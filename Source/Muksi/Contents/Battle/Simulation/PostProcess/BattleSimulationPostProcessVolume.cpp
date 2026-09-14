#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"

#include "Materials/MaterialInterface.h"

ABattleSimulationPostProcessVolume::ABattleSimulationPostProcessVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bUnbound = true;
	bEnabled = false;
	BlendWeight = 0.0f;
}


void ABattleSimulationPostProcessVolume::InitializeSimulationPostProcess(
	UMaterialInterface* InMaterial,
	float InBlendWeight,
	bool bInLockAutoExposure,
	float InFixedExposure)
{
	SimulationPostProcessMaterial = InMaterial;
	SimulationBlendWeight = FMath::Clamp(InBlendWeight, 0.0f, 1.0f);

	// This is a dedicated runtime volume. Rebuild its blendable list from the
	// manager-owned configuration so Blueprint defaults on APostProcessVolume
	// are not required.
	Settings.WeightedBlendables.Array.Reset();

	// Equal Auto Exposure min/max values disable eye adaptation.
	// The editor may label these values as EV100 when the extended luminance range is enabled,
	// but FPostProcessSettings keeps the MinBrightness/MaxBrightness C++ member names.
	Settings.bOverride_AutoExposureMinBrightness = bInLockAutoExposure;
	Settings.bOverride_AutoExposureMaxBrightness = bInLockAutoExposure;
	if (bInLockAutoExposure)
	{
		Settings.AutoExposureMinBrightness = InFixedExposure;
		Settings.AutoExposureMaxBrightness = InFixedExposure;
	}

	EnsureSimulationPostProcessMaterial();
}

void ABattleSimulationPostProcessVolume::ActivateSimulationPostProcess()
{
	EnsureSimulationPostProcessMaterial();
	BlendWeight = SimulationBlendWeight;
	bEnabled = true;
}

void ABattleSimulationPostProcessVolume::DeactivateSimulationPostProcess()
{
	BlendWeight = 0.0f;
	bEnabled = false;
}

bool ABattleSimulationPostProcessVolume::IsSimulationPostProcessActive() const
{
	return bEnabled && BlendWeight > 0.0f;
}

void ABattleSimulationPostProcessVolume::EnsureSimulationPostProcessMaterial()
{
	if (!IsValid(SimulationPostProcessMaterial))
		return;

	// Avoid appending the same blendable every time the presentation is re-entered.
	for (FWeightedBlendable& Blendable : Settings.WeightedBlendables.Array)
	{
		if (Blendable.Object.Get() == SimulationPostProcessMaterial.Get())
		{
			Blendable.Weight = 1.0f;
			return;
		}
	}

	Settings.AddBlendable(SimulationPostProcessMaterial, 1.0f);
}
