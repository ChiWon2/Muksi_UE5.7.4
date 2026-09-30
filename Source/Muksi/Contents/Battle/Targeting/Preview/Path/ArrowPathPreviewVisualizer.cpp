#include "Muksi/Contents/Battle/Targeting/Preview/Path/ArrowPathPreviewVisualizer.h"

#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/PathPreviewDirectionUtils.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/ArrowPathPreviewData.h"


void UArrowPathPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	Super::Initialize(InPreviewActor);

	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
		return;

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

	if (!HasPreviewActor() || !Context.IsValid() || !Context.IsStepValid())
		return;

	if (!IsPathPreviewDataValid(Context.StepData->Presentation.Visualizers.Path.Data))
		return;

	const FArrowPathPreviewData* Data = Context.StepData->Presentation.Visualizers.Path.Data.GetPtr<FArrowPathPreviewData>();

	if (!Data || !ArrowPreviewMesh || !Context.HasOriginCoord())
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
		FVector RawEndLocation = FVector::ZeroVector;

		if (!MuksiPathPreview::GetGroupEndLocation(Context, Group, RawEndLocation))
			continue;

		RawEndLocation.Z += PreviewHeightOffset;

		FVector AimDirection = FVector::ZeroVector;

		if (!MuksiPathPreview::GetPathDirection(Context, Group, *Data, StartLocation, RawEndLocation, AimDirection))
			continue;

		const float RawLength = FVector::Dist2D(StartLocation, RawEndLocation);
		const float TotalLength = Data->bUseFixedLength ? FMath::Max(0.0f, Data->Length) : RawLength;

		if (TotalLength <= KINDA_SMALL_NUMBER)
			continue;

		const FVector EndLocation = StartLocation + AimDirection * TotalLength;
		const float ArrowHeadLength = FMath::Min(FMath::Max(0.0f, Data->ArrowHeadLength), TotalLength);
		const float ArrowHeadWidth = FMath::Max(0.0f, Data->ArrowHeadWidth);
		const FVector BodyEndLocation = EndLocation - AimDirection * ArrowHeadLength;
		const FVector ArrowLocation = EndLocation - AimDirection * ArrowHeadLength * 0.5f;

		if (StraightPreviewMesh && FVector::DistSquared(StartLocation, BodyEndLocation) > KINDA_SMALL_NUMBER)
		{
			USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

			if (PathMeshComponent)
			{
				const FVector LocalStartLocation = ActorTransform.InverseTransformPosition(StartLocation);
				const FVector LocalEndLocation = ActorTransform.InverseTransformPosition(BodyEndLocation);
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

		UStaticMeshComponent* ArrowMeshComponent = PreviewActorInstance->CreateArrowMeshComponent();

		if (!ArrowMeshComponent)
			continue;

		const float ArrowLengthScale = ArrowHeadLength / PreviewMeshBaseSize;
		const float ArrowWidthScale = ArrowHeadWidth / PreviewMeshBaseSize;

		ArrowMeshComponent->SetStaticMesh(ArrowPreviewMesh);

		if (ArrowPreviewMaterial)
			ArrowMeshComponent->SetMaterial(0, ArrowPreviewMaterial);

		ArrowMeshComponent->SetWorldLocation(ArrowLocation);
		ArrowMeshComponent->SetWorldRotation(AimDirection.Rotation());
		ArrowMeshComponent->SetWorldScale3D(FVector(ArrowLengthScale, ArrowWidthScale, 1.0f));
	}
}

const UScriptStruct* UArrowPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FArrowPathPreviewData::StaticStruct();
}
