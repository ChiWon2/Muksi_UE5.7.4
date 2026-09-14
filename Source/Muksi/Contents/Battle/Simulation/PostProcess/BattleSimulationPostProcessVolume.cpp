#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"

#include "Curves/CurveFloat.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace BattleSimulationPostProcessParameters
{
	static const FName TransitionOrigin(TEXT("SimulationTransitionOrigin"));
	static const FName TransitionRadius(TEXT("SimulationTransitionRadius"));
	static const FName TransitionProgress(TEXT("SimulationTransitionProgress"));
}

ABattleSimulationPostProcessVolume::ABattleSimulationPostProcessVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bUnbound = true;
	bEnabled = false;
	BlendWeight = 0.0f;
}

void ABattleSimulationPostProcessVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bTransitionPlaying)
	{
		PrimaryActorTick.SetTickFunctionEnable(false);
		return;
	}

	if (TransitionDuration <= KINDA_SMALL_NUMBER)
	{
		TransitionAlpha = bTransitionEntering ? 1.0f : 0.0f;
		UpdateTransitionMaterialParameters();
		CompleteTransition();
		return;
	}

	const float Direction = bTransitionEntering ? 1.0f : -1.0f;
	TransitionAlpha = FMath::Clamp(
		TransitionAlpha + (DeltaSeconds / TransitionDuration) * Direction,
		0.0f,
		1.0f);

	UpdateTransitionMaterialParameters();

	if ((bTransitionEntering && TransitionAlpha >= 1.0f) ||
		(!bTransitionEntering && TransitionAlpha <= 0.0f))
	{
		CompleteTransition();
	}
}

void ABattleSimulationPostProcessVolume::InitializeSimulationPostProcess(
	UMaterialInterface* InMaterial,
	float InBlendWeight,
	bool bInLockAutoExposure,
	float InFixedExposure,
	float InTransitionDuration,
	float InTransitionMaxRadius,
	UCurveFloat* InTransitionCurve)
{
	const bool bMaterialChanged = SimulationPostProcessMaterial.Get() != InMaterial;
	SimulationPostProcessMaterial = InMaterial;
	SimulationBlendWeight = FMath::Clamp(InBlendWeight, 0.0f, 1.0f);
	TransitionDuration = FMath::Max(0.0f, InTransitionDuration);
	TransitionMaxRadius = FMath::Max(0.0f, InTransitionMaxRadius);
	TransitionCurve = InTransitionCurve;

	// This is a dedicated runtime volume. Rebuild its blendable list from the
	// manager-owned configuration so Blueprint defaults on APostProcessVolume
	// are not required.
	Settings.WeightedBlendables.Array.Reset();
	if (bMaterialChanged)
		SimulationPostProcessMID = nullptr;

	// Equal Auto Exposure min/max values disable eye adaptation.
	Settings.bOverride_AutoExposureMinBrightness = bInLockAutoExposure;
	Settings.bOverride_AutoExposureMaxBrightness = bInLockAutoExposure;
	if (bInLockAutoExposure)
	{
		Settings.AutoExposureMinBrightness = InFixedExposure;
		Settings.AutoExposureMaxBrightness = InFixedExposure;
	}

	EnsureSimulationPostProcessMaterial();
	UpdateTransitionMaterialParameters();
}

void ABattleSimulationPostProcessVolume::ActivateSimulationPostProcess(const FVector& InTransitionOrigin)
{
	EnsureSimulationPostProcessMaterial();
	TransitionOrigin = InTransitionOrigin;

	BlendWeight = SimulationBlendWeight;
	bEnabled = true;
	StartTransition(true);
}

void ABattleSimulationPostProcessVolume::DeactivateSimulationPostProcess()
{
	if (!bEnabled)
	{
		Destroy();
		return;
	}

	StartTransition(false);
}

bool ABattleSimulationPostProcessVolume::IsSimulationPostProcessActive() const
{
	return bEnabled && BlendWeight > 0.0f;
}

void ABattleSimulationPostProcessVolume::EnsureSimulationPostProcessMaterial()
{
	if (!IsValid(SimulationPostProcessMaterial))
		return;

	if (!IsValid(SimulationPostProcessMID))
	{
		SimulationPostProcessMID = UMaterialInstanceDynamic::Create(SimulationPostProcessMaterial, this);
	}

	if (!IsValid(SimulationPostProcessMID))
		return;

	for (FWeightedBlendable& Blendable : Settings.WeightedBlendables.Array)
	{
		if (Blendable.Object.Get() == SimulationPostProcessMID.Get())
		{
			Blendable.Weight = 1.0f;
			return;
		}
	}

	Settings.AddBlendable(SimulationPostProcessMID, 1.0f);
}

void ABattleSimulationPostProcessVolume::StartTransition(bool bEntering)
{
	bTransitionEntering = bEntering;
	bTransitionPlaying = true;
	PrimaryActorTick.SetTickFunctionEnable(true);

	// Preserve the current alpha when direction changes so an interrupted
	// transition reverses smoothly instead of popping back to an endpoint.
	UpdateTransitionMaterialParameters();

	if (TransitionDuration <= KINDA_SMALL_NUMBER)
	{
		TransitionAlpha = bEntering ? 1.0f : 0.0f;
		UpdateTransitionMaterialParameters();
		CompleteTransition();
	}
}

void ABattleSimulationPostProcessVolume::UpdateTransitionMaterialParameters()
{
	if (!IsValid(SimulationPostProcessMID))
		return;

	const float EasedAlpha = EvaluateTransitionAlpha(TransitionAlpha);
	const float CurrentRadius = FMath::Lerp(0.0f, TransitionMaxRadius, EasedAlpha);

	SimulationPostProcessMID->SetVectorParameterValue(
		BattleSimulationPostProcessParameters::TransitionOrigin,
		FLinearColor(TransitionOrigin.X, TransitionOrigin.Y, TransitionOrigin.Z, 1.0f));
	SimulationPostProcessMID->SetScalarParameterValue(
		BattleSimulationPostProcessParameters::TransitionRadius,
		CurrentRadius);
	SimulationPostProcessMID->SetScalarParameterValue(
		BattleSimulationPostProcessParameters::TransitionProgress,
		TransitionAlpha);
}

void ABattleSimulationPostProcessVolume::CompleteTransition()
{
	bTransitionPlaying = false;
	PrimaryActorTick.SetTickFunctionEnable(false);

	if (bTransitionEntering)
		return;

	BlendWeight = 0.0f;
	bEnabled = false;
	Destroy();
}

float ABattleSimulationPostProcessVolume::EvaluateTransitionAlpha(float InAlpha) const
{
	const float ClampedAlpha = FMath::Clamp(InAlpha, 0.0f, 1.0f);
	if (IsValid(TransitionCurve))
		return FMath::Clamp(TransitionCurve->GetFloatValue(ClampedAlpha), 0.0f, 1.0f);

	// SmoothStep is the default when no curve asset is configured.
	return ClampedAlpha * ClampedAlpha * (3.0f - 2.0f * ClampedAlpha);
}
