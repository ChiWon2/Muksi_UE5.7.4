#include "Muksi/Contents/Battle/Targeting/Preview/Path/ArrowPathPreviewVisualizer.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/ArrowPathPreviewData.h"

void UArrowPathPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	StraightPreviewMesh = Settings->StraightPreviewMesh.LoadSynchronous();
	ArrowPreviewMesh = Settings->ArrowPreviewMesh.LoadSynchronous();
	StraightPreviewMaterial = Settings->StraightPreviewMaterial.LoadSynchronous();
	ArrowPreviewMaterial = Settings->ArrowPreviewMaterial.LoadSynchronous();
	PreviewHeightOffset = Settings->PreviewHeightOffset;
	PreviewLineThickness = Settings->PreviewLineThickness;
	PreviewMeshBaseSize = FMath::Max(KINDA_SMALL_NUMBER, Settings->PreviewMeshBaseSize);
}

void UArrowPathPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
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

	const FArrowPathPreviewData* Data = Context.StepData->PathPreviewData.GetPtr<FArrowPathPreviewData>();

	if (!Data || !ArrowPreviewMesh)
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
	const float TotalLength = FVector::Distance(StartLocation, EndLocation);

	if (TotalLength <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	USplineComponent* PathSpline = PreviewActorInstance->GetPathSpline();
	UStaticMeshComponent* ArrowMeshComponent = PreviewActorInstance->GetArrowPreviewMesh();

	if (!PathSpline || !ArrowMeshComponent)
	{
		return;
	}

	const float ArrowHeadLength = FMath::Min(FMath::Max(0.0f, Data->ArrowHeadLength), TotalLength);
	const float ArrowHeadWidth = FMath::Max(0.0f, Data->ArrowHeadWidth);
	const FVector BodyEndLocation = EndLocation - AimDirection * ArrowHeadLength;
	const FVector ArrowLocation = EndLocation - AimDirection * ArrowHeadLength * 0.5f;

	TArray<FVector> SplinePoints;
	SplinePoints.Add(StartLocation);
	SplinePoints.Add(EndLocation);
	PathSpline->SetSplinePoints(SplinePoints, ESplineCoordinateSpace::World, true);

	if (StraightPreviewMesh && FVector::DistSquared(StartLocation, BodyEndLocation) > KINDA_SMALL_NUMBER)
	{
		USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

		if (PathMeshComponent)
		{
			const FTransform ActorTransform = PreviewActorInstance->GetActorTransform();
			const FVector LocalStartLocation = ActorTransform.InverseTransformPosition(StartLocation);
			const FVector LocalEndLocation = ActorTransform.InverseTransformPosition(BodyEndLocation);
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
		}
	}

	const float ArrowLengthScale = ArrowHeadLength / PreviewMeshBaseSize;
	const float ArrowWidthScale = ArrowHeadWidth / PreviewMeshBaseSize;

	ArrowMeshComponent->SetStaticMesh(ArrowPreviewMesh);

	if (ArrowPreviewMaterial)
	{
		ArrowMeshComponent->SetMaterial(0, ArrowPreviewMaterial);
	}

	ArrowMeshComponent->SetWorldLocation(ArrowLocation);
	ArrowMeshComponent->SetWorldRotation(AimDirection.Rotation());
	ArrowMeshComponent->SetWorldScale3D(FVector(ArrowLengthScale, ArrowWidthScale, 1.0f));
	ArrowMeshComponent->SetVisibility(true);

	if (Context.PreviewResult)
	{
		PreviewActorInstance->SetPathGridCoords(Context.PreviewResult->PathCoords);
	}
}

const UScriptStruct* UArrowPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FArrowPathPreviewData::StaticStruct();
}