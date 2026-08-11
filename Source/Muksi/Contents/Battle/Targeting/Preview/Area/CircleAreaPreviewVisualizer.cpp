#include "Muksi/Contents/Battle/Targeting/Preview/Area/CircleAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Circle/CirclePatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UCircleAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	CirclePreviewMesh = Settings->CirclePreviewMesh.LoadSynchronous();
	CirclePreviewMaterial = Settings->CirclePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UCircleAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.ResolvedTargeting)
	{
		return;
	}

	if (!IsPatternDataValid(Context.StepData->Pattern.PatternData))
	{
		return;
	}

	const FCirclePatternData* Data = Context.StepData->Pattern.PatternData.GetPtr<FCirclePatternData>();
	// Use the step explicitly bound to this preview session.
	// Runtime presentation can display multiple steps, so the overall resolved
	// result's last step is not necessarily the step this visualizer represents.
	const FTargetingStepResult* StepResult = Context.StepResult;

	if (!Data || !StepResult || !StepResult->HasSelectedCoord())
	{
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	UStaticMeshComponent* PreviewMeshComponent = PreviewActorInstance->GetAreaPreviewMesh();

	if (!PreviewMeshComponent)
	{
		return;
	}

	PreviewActorInstance->SetAreaGridCoords(Context.ResolvedTargeting->AffectedCoords);
	PreviewMeshComponent->SetVisibility(false);

	if (!CirclePreviewMesh)
	{
		return;
	}

	// CirclePattern is resolved from SelectedCoord. Raw AimWorldLocation may be an
	// arbitrary point inside the tile, so using it here makes the mesh disagree with
	// the indicator and with reveal/runtime previews.
	FVector CenterLocation = FVector::ZeroVector;
	if (!Context.GridManager->GetPresentationWorldLocationByCoord(StepResult->SelectedCoord, CenterLocation)) return;
	const float WorldRadius = CalculateWorldRadius(Context, Data->Radius);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = CenterLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);

	PreviewMeshComponent->SetStaticMesh(CirclePreviewMesh);

	if (CirclePreviewMaterial)
	{
		PreviewMeshComponent->SetMaterial(0, CirclePreviewMaterial);
	}

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}
float UCircleAreaPreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context, int32 GridRange) const
{
	if (!Context.GridManager)
	{
		return 0.0f;
	}

	return Context.GridManager->GetWorldRadiusByGridRange(FMath::Max(0, GridRange), true);
}
const UScriptStruct* UCircleAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FCirclePatternData::StaticStruct();
}
