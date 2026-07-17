#include "Muksi/Contents/Battle/Targeting/Preview/Selection/CircleRangePreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UCircleRangePreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	CirclePreviewMesh = Settings->CirclePreviewMesh.LoadSynchronous();
	RangePreviewMaterial = Settings->RangePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UCircleRangePreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	if (!HasPreviewActor() || !Context.IsValid() || !Context.StepContext)
	{
		ClearPreview();
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	UStaticMeshComponent* PreviewMeshComponent = PreviewActorInstance->GetSelectionPreviewMesh();

	if (!PreviewMeshComponent)
	{
		ClearPreview();
		return;
	}

	//PreviewActorInstance->SetSelectionGridCoords(Context.StepContext->SelectableCoords);
	PreviewMeshComponent->SetVisibility(false);

	if (!Context.StepContext->HasOriginCoord() || Context.StepContext->SelectableCoords.IsEmpty() || !CirclePreviewMesh)
	{
		return;
	}

	const float WorldRadius = CalculateWorldRadius(Context);

	if (WorldRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = Context.StepContext->OriginWorldLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);

	PreviewMeshComponent->SetStaticMesh(CirclePreviewMesh);

	if (RangePreviewMaterial)
	{
		PreviewMeshComponent->SetMaterial(0, RangePreviewMaterial);
	}

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}

float UCircleRangePreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context) const
{
	if (!Context.GridManager || !Context.StepContext)
	{
		return 0.0f;
	}

	float MaximumDistance = 0.0f;

	for (const FIntPoint& Coord : Context.StepContext->SelectableCoords)
	{
		const FBattleGridCell* Cell = Context.GridManager->GetCell(Coord);

		if (!Cell)
		{
			continue;
		}

		FVector Difference = Cell->WorldLocation - Context.StepContext->OriginWorldLocation;
		Difference.Z = 0.0f;
		MaximumDistance = FMath::Max(MaximumDistance, Difference.Size());
	}

	return MaximumDistance + FMath::Max(0.0f, Context.GridManager->HexRadius);
}