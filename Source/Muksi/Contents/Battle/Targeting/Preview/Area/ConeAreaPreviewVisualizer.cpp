#include "Muksi/Contents/Battle/Targeting/Preview/Area/ConeAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Cone/ConePatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UConeAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	ConePreviewMesh = Settings->ConePreviewMesh.LoadSynchronous();
	ConePreviewMaterial = Settings->ConePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);

	if (ConePreviewMaterial)
	{
		ConeDynamicMaterial = UMaterialInstanceDynamic::Create(ConePreviewMaterial, this);
	}
}

void UConeAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid())
	{
		return;
	}

	if (!IsPatternDataValid(Context.StepData->Pattern.PatternData))
	{
		return;
	}

	const FConePatternData* Data = Context.StepData->Pattern.PatternData.GetPtr<FConePatternData>();
	// Use the step explicitly bound to this preview session.
	// Runtime presentation can display multiple steps, so the overall resolved
	// result's last step is not necessarily the step this visualizer represents.
	
	if (!Data || !Context.HasOriginCoord() || !Context.HasDirection())
	{
		return;
	}

	if (!Context.GridManager->IsValidCoord(Context.GetOriginCoord())) return;

	const FVector LogicalOriginLocation = Context.GridManager->GetWorldLocationByCoord(Context.GetOriginCoord());

	// Area previews must visualize the exact resolved pattern result.
	// AimWorldLocation is presentation input for path/selection previews only; using it
	// here can point the cone away from StepResult.Direction during enemy/reveal/runtime phases.
	const FHexOffsetCoord ResolvedAimCoord = FHexGridMath::GetNeighborCoord(Context.GetOriginCoord(), Context.GetDirection());
	FVector ResolvedDirection = Context.GridManager->GetWorldLocationByCoord(ResolvedAimCoord) - LogicalOriginLocation;
	ResolvedDirection.Z = 0.0f;

	if (!ResolvedDirection.Normalize())
	{
		return;
	}

	const float TargetYaw = ResolvedDirection.Rotation().Yaw;

	// Do not interpolate from an unrelated previous phase/session direction.
	// The pattern and indicator are discrete hex-direction results, so the area mesh
	// must snap to the same direction on every update.
	CurrentPreviewYaw = TargetYaw;
	bHasPreviewYaw = true;

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	UStaticMeshComponent* PreviewMeshComponent = PreviewActorInstance->GetAreaPreviewMesh();

	if (!PreviewMeshComponent)
	{
		return;
	}
	PreviewMeshComponent->SetVisibility(false);

	if (!ConePreviewMesh)
	{
		return;
	}

	const float WorldRadius = CalculateWorldRadius(Context, Data->Range);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	FVector PresentationOriginLocation = FVector::ZeroVector;
	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetOriginCoord(), PresentationOriginLocation)) return;
	const FVector PreviewLocation = PresentationOriginLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);
	const FRotator PreviewRotation(0.0f, CurrentPreviewYaw, 0.0f);

	PreviewMeshComponent->SetStaticMesh(ConePreviewMesh);

	if (ConeDynamicMaterial)
	{
		ConeDynamicMaterial->SetScalarParameterValue(TEXT("ConeAngle"), FMath::Clamp(Data->Angle, 1.0f, 360.0f));
		PreviewMeshComponent->SetMaterial(0, ConeDynamicMaterial);
	}
	else if (ConePreviewMaterial)
	{
		PreviewMeshComponent->SetMaterial(0, ConePreviewMaterial);
	}

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(PreviewRotation);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}
float UConeAreaPreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context, int32 GridRange) const
{
	if (!Context.GridManager)
	{
		return 0.0f;
	}

	return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(1, GridRange), true);
}

const UScriptStruct* UConeAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FConePatternData::StaticStruct();
}
