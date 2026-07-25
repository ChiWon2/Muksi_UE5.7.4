#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"

ABattleSimulationPostProcessVolume::ABattleSimulationPostProcessVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bUnbound = true;
	bEnabled = false;
	BlendWeight = 0.0f;
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = GrayscaleSaturation;
	Settings.bOverride_ColorContrast = true;
	Settings.ColorContrast = GrayscaleContrast;
}

void ABattleSimulationPostProcessVolume::ActivateSimulationPostProcess()
{
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = GrayscaleSaturation;
	Settings.bOverride_ColorContrast = true;
	Settings.ColorContrast = GrayscaleContrast;
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