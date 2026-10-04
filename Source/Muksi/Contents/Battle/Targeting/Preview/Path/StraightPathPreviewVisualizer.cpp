#include "Muksi/Contents/Battle/Targeting/Preview/Path/StraightPathPreviewVisualizer.h"

#include "Components/SplineMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/PathPreviewDirectionUtils.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/StraightPathPreviewData.h"


void UStraightPathPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
		return;

	StraightPreviewMesh = Settings->StraightPreviewMesh.LoadSynchronous();
	StraightPreviewMaterial = Settings->StraightPreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewLineThickness = Settings->PreviewLineThickness;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UStraightPathPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.IsStepValid())
		return;

	if (!IsPathPreviewDataValid(Context.StepData->Presentation.Visualizers.Path.Data))
		return;

	const FStraightPathPreviewData* Data = Context.StepData->Presentation.Visualizers.Path.Data.GetPtr<FStraightPathPreviewData>();

	if (!Data || !StraightPreviewMesh || !Context.HasOriginCoord())
		return;

	const TArray<FTargetingGroup>* Groups = Context.GetGroups();

	if (!Groups)
		return;

	FVector StartLocation = FVector::ZeroVector;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetOriginCoord(), StartLocation))
		return;

	StartLocation.Z += PreviewHeightOffset;

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	const FTransform ActorTransform = PreviewActorInstance->GetActorTransform();
	const float ThicknessScale = FMath::Max(KINDA_SMALL_NUMBER, PreviewLineThickness / PreviewMeshBaseSize);

	for (const FTargetingGroup& Group : *Groups)
	{
		FVector EndLocation = FVector::ZeroVector;

		if (!MuksiPathPreview::GetPathRangeEndLocation(Context, Group, StartLocation, EndLocation))
			continue;

		if (FVector::DistSquared(StartLocation, EndLocation) <= KINDA_SMALL_NUMBER)
			continue;

		USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

		if (!PathMeshComponent)
			continue;

		const FVector LocalStartLocation = ActorTransform.InverseTransformPosition(StartLocation);
		const FVector LocalEndLocation = ActorTransform.InverseTransformPosition(EndLocation);
		const FVector LocalTangent = LocalEndLocation - LocalStartLocation;

		PathMeshComponent->SetStaticMesh(StraightPreviewMesh);
		PathMeshComponent->SetForwardAxis(ESplineMeshAxis::X, false);
		PathMeshComponent->SetStartAndEnd(LocalStartLocation, LocalTangent, LocalEndLocation, LocalTangent, false);
		PathMeshComponent->SetStartScale(FVector2D(ThicknessScale, 1.0f), false);
		PathMeshComponent->SetEndScale(FVector2D(ThicknessScale, 1.0f), false);

		if (StraightPreviewMaterial)
			PathMeshComponent->SetMaterial(0, StraightPreviewMaterial);

		PathMeshComponent->UpdateMesh();
	}
}

const UScriptStruct* UStraightPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FStraightPathPreviewData::StaticStruct();
}
