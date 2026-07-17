#include "Muksi/Contents/Battle/Targeting/Preview/Area/CircleAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
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

	if (!HasPreviewActor() || !Context.IsValid() || !Context.TargetingData || !Context.PreviewResult)
	{
		return;
	}

	if (!IsPatternDataValid(Context.TargetingData->FinalPatternData))
	{
		return;
	}

	const FCirclePatternData* Data = Context.TargetingData->FinalPatternData.GetPtr<FCirclePatternData>();
	const FTargetingStepContext* StepContext = Context.PreviewResult->GetLastStepContext();

	if (!Data || !StepContext || !StepContext->HasSelectedCoord())
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

	if (!CirclePreviewMesh)
	{
		return;
	}

	const FVector CenterLocation = StepContext->SelectedWorldLocation;
	const float WorldRadius = CalculateWorldRadius(Context, CenterLocation);

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

float UCircleAreaPreviewVisualizer::CalculateWorldRadius(const FTargetingPreviewContext& Context, const FVector& CenterLocation) const
{
	if (!Context.GridManager || !Context.PreviewResult)
	{
		return 0.0f;
	}

	float MaximumDistance = 0.0f;

	for (const FIntPoint& Coord : Context.PreviewResult->AffectedCoords)
	{
		const FBattleGridCell* Cell = Context.GridManager->GetCell(Coord);

		if (!Cell)
		{
			continue;
		}

		FVector Difference = Cell->WorldLocation - CenterLocation;
		Difference.Z = 0.0f;
		MaximumDistance = FMath::Max(MaximumDistance, Difference.Size());
	}

	return MaximumDistance + FMath::Max(0.0f, Context.GridManager->HexRadius);
}

const UScriptStruct* UCircleAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FCirclePatternData::StaticStruct();
}