#include "Muksi/Contents/Battle/Targeting/Preview/Area/ConeAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
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

	if (!HasPreviewActor() || !Context.IsValid() || !Context.StepData || !Context.PreviewResult)
	{
		return;
	}

	if (!IsPatternDataValid(Context.StepData->PatternData))
	{
		return;
	}

	const FConePatternData* Data = Context.StepData->PatternData.GetPtr<FConePatternData>();
	const FTargetingStepContext* StepContext = Context.PreviewResult->GetLastStepContext();

	if (!Data || !StepContext || !StepContext->HasOriginCoord())
	{
		return;
	}

	FVector AimDirection = StepContext->AimWorldLocation - StepContext->OriginWorldLocation;
	AimDirection.Z = 0.0f;

	if (!AimDirection.Normalize())
	{
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	UStaticMeshComponent* PreviewMeshComponent = PreviewActorInstance->GetAreaPreviewMesh();

	if (!PreviewMeshComponent)
	{
		return;
	}

	PreviewActorInstance->SetAreaGridCoords(Context.PreviewResult->AffectedCoords);
	PreviewMeshComponent->SetVisibility(false);

	if (!ConePreviewMesh)
	{
		return;
	}

	const FVector OriginLocation = StepContext->OriginWorldLocation;
	const float WorldRadius = CalculateWorldRadius(Context, Data->Range);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = OriginLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);
	const FRotator PreviewRotation(0.0f, AimDirection.Rotation().Yaw, 0.0f);

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

	return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(0, GridRange), true);
}

const UScriptStruct* UConeAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FConePatternData::StaticStruct();
}