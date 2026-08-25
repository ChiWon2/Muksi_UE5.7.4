#include "Muksi/Contents/Battle/Targeting/Preview/Area/PointAreaPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UPointAreaPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();
	if (!Settings) return;

	PointPreviewMesh = Settings->CirclePreviewMesh.LoadSynchronous();
	PointPreviewMaterial = Settings->CirclePreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UPointAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid()) return;
	if (!IsPatternDataValid(Context.StepData->Pattern.PatternData)) return;

		if (!Context.HasTargetCoord()) return;

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	UStaticMeshComponent* PreviewMeshComponent = PreviewActorInstance->GetAreaPreviewMesh();
	if (!PreviewMeshComponent) return;

	PreviewMeshComponent->SetVisibility(false);
	if (!PointPreviewMesh) return;

	FVector CenterLocation = FVector::ZeroVector;
	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetTargetCoord(), CenterLocation)) return;

	const float WorldRadius = Context.GridManager->GetWorldRadiusByGridRange(0, true);
	if (WorldRadius <= KINDA_SMALL_NUMBER) return;

	const float PreviewScale = WorldRadius * 2.0f / PreviewMeshBaseSize;
	const FVector PreviewLocation = CenterLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);

	PreviewMeshComponent->SetStaticMesh(PointPreviewMesh);
	if (PointPreviewMaterial)
		PreviewMeshComponent->SetMaterial(0, PointPreviewMaterial);

	PreviewMeshComponent->SetWorldLocation(PreviewLocation);
	PreviewMeshComponent->SetWorldRotation(FRotator::ZeroRotator);
	PreviewMeshComponent->SetWorldScale3D(FVector(PreviewScale, PreviewScale, 1.0f));
	PreviewMeshComponent->SetVisibility(true);
}

const UScriptStruct* UPointAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}
