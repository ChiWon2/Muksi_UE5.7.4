#include "Muksi/Contents/Battle/Targeting/Preview/Path/ArcPathPreviewVisualizer.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/ArcPathPreviewData.h"

void UArcPathPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	StraightPreviewMesh = Settings->StraightPreviewMesh.LoadSynchronous();
	ArcPreviewMaterial = Settings->ArcPreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewLineThickness = Settings->PreviewLineThickness;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UArcPathPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
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

	const FArcPathPreviewData* Data = Context.StepData->PathPreviewData.GetPtr<FArcPathPreviewData>();

	if (!Data || !StraightPreviewMesh)
	{
		return;
	}

	FVector StartLocation = Context.StepContext->OriginWorldLocation;
	FVector EndLocation = Context.StepContext->HasSelectedCoord() ? Context.StepContext->SelectedWorldLocation : Context.StepContext->AimWorldLocation;

	StartLocation.Z += PreviewHeightOffset;
	EndLocation.Z += PreviewHeightOffset;

	if (FVector::DistSquared2D(StartLocation, EndLocation) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	USplineComponent* PathSpline = PreviewActorInstance->GetPathSpline();

	if (!PathSpline)
	{
		return;
	}

	const float ArcHeight = FMath::Max(0.0f, Data->ArcHeight);
	const int32 SegmentCount = FMath::Clamp(Data->SegmentCount, 2, 128);
	const float ThicknessScale = FMath::Max(KINDA_SMALL_NUMBER, PreviewLineThickness / PreviewMeshBaseSize);

	PathSpline->ClearSplinePoints(false);

	for (int32 PointIndex = 0; PointIndex <= SegmentCount; ++PointIndex)
	{
		const float Alpha = static_cast<float>(PointIndex) / static_cast<float>(SegmentCount);
		FVector PointLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
		PointLocation.Z += 4.0f * ArcHeight * Alpha * (1.0f - Alpha);
		PathSpline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World, false);
		PathSpline->SetSplinePointType(PointIndex, ESplinePointType::CurveClamped, false);
	}

	PathSpline->UpdateSpline();

	for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
	{
		USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

		if (!PathMeshComponent)
		{
			continue;
		}

		const FVector StartPoint = PathSpline->GetLocationAtSplinePoint(SegmentIndex, ESplineCoordinateSpace::Local);
		const FVector StartTangent = PathSpline->GetTangentAtSplinePoint(SegmentIndex, ESplineCoordinateSpace::Local);
		const FVector EndPoint = PathSpline->GetLocationAtSplinePoint(SegmentIndex + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = PathSpline->GetTangentAtSplinePoint(SegmentIndex + 1, ESplineCoordinateSpace::Local);

		PathMeshComponent->SetStaticMesh(StraightPreviewMesh);
		PathMeshComponent->SetForwardAxis(ESplineMeshAxis::X, false);
		PathMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);
		PathMeshComponent->SetStartScale(FVector2D(ThicknessScale, 1.0f), false);
		PathMeshComponent->SetEndScale(FVector2D(ThicknessScale, 1.0f), false);

		if (ArcPreviewMaterial)
		{
			PathMeshComponent->SetMaterial(0, ArcPreviewMaterial);
		}

		PathMeshComponent->UpdateMesh();
	}

	if (Context.PreviewResult)
	{
		PreviewActorInstance->SetPathGridCoords(Context.PreviewResult->PathCoords);
	}
}

const UScriptStruct* UArcPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FArcPathPreviewData::StaticStruct();
}