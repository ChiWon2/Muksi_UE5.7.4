#include "Muksi/Contents/Battle/Targeting/Preview/Path/StraightPathPreviewVisualizer.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/StraightPathPreviewData.h"

void UStraightPathPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	StraightPreviewMesh = Settings->StraightPreviewMesh.LoadSynchronous();
	StraightPreviewMaterial = Settings->StraightPreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewLineThickness = Settings->PreviewLineThickness;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UStraightPathPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.StepData || !Context.StepContext)
	{
		return;
	}

	if (!IsPathPreviewDataValid(Context.StepData->PathPreviewData))
	{
		return;
	}

	const FStraightPathPreviewData* Data = Context.StepData->PathPreviewData.GetPtr<FStraightPathPreviewData>();

	if (!Data || !StraightPreviewMesh)
	{
		return;
	}

	FVector StartLocation = Context.StepContext->OriginWorldLocation;
	FVector AimLocation = Context.StepContext->AimWorldLocation;
	FVector AimDirection = AimLocation - StartLocation;

	AimDirection.Z = 0.0f;

	if (!AimDirection.Normalize())
	{
		return;
	}

	StartLocation.Z += PreviewHeightOffset;
	AimLocation.Z = StartLocation.Z;

	const float SafeLength = FMath::Max(0.0f, Data->Length);
	const FVector EndLocation = Data->bUseFixedLength ? StartLocation + AimDirection * SafeLength : AimLocation;

	if (FVector::DistSquared(StartLocation, EndLocation) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	USplineComponent* PathSpline = PreviewActorInstance->GetPathSpline();

	if (!PathSpline)
	{
		return;
	}

	TArray<FVector> SplinePoints;
	SplinePoints.Add(StartLocation);
	SplinePoints.Add(EndLocation);
	PathSpline->SetSplinePoints(SplinePoints, ESplineCoordinateSpace::World, true);

	USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

	if (!PathMeshComponent)
	{
		return;
	}

	const FTransform ActorTransform = PreviewActorInstance->GetActorTransform();
	const FVector LocalStartLocation = ActorTransform.InverseTransformPosition(StartLocation);
	const FVector LocalEndLocation = ActorTransform.InverseTransformPosition(EndLocation);
	const FVector LocalTangent = LocalEndLocation - LocalStartLocation;
	const float ThicknessScale = FMath::Max(KINDA_SMALL_NUMBER, PreviewLineThickness / PreviewMeshBaseSize);

	PathMeshComponent->SetStaticMesh(StraightPreviewMesh);
	PathMeshComponent->SetForwardAxis(ESplineMeshAxis::X, false);
	PathMeshComponent->SetStartAndEnd(LocalStartLocation, LocalTangent, LocalEndLocation, LocalTangent, false);
	PathMeshComponent->SetStartScale(FVector2D(ThicknessScale, 1.0f), false);
	PathMeshComponent->SetEndScale(FVector2D(ThicknessScale, 1.0f), false);

	if (StraightPreviewMaterial)
	{
		PathMeshComponent->SetMaterial(0, StraightPreviewMaterial);
	}

	PathMeshComponent->UpdateMesh();

	if (Context.PreviewResult)
	{
		PreviewActorInstance->SetPathGridCoords(Context.PreviewResult->PathCoords);
	}
}

const UScriptStruct* UStraightPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FStraightPathPreviewData::StaticStruct();
}