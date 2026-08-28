#include "Muksi/Contents/Battle/Targeting/Preview/Path/ArcPathPreviewVisualizer.h"

#include "Components/SplineMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
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

	if (!HasPreviewActor() || !Context.IsValid())
		return;

	if (!IsPathPreviewDataValid(Context.StepData->Presentation.Visualizers.Path.Data))
		return;

	const FArcPathPreviewData* Data = Context.StepData->Presentation.Visualizers.Path.Data.GetPtr<FArcPathPreviewData>();

	if (!Data || !StraightPreviewMesh || !Context.HasOriginCoord() || !Context.HasTargetCoord())
		return;

	FVector StartLocation = FVector::ZeroVector;
	FVector EndLocation = FVector::ZeroVector;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetOriginCoord(), StartLocation))
		return;

	if (!Context.GridManager->GetPresentationWorldLocationByCoord(Context.GetTargetCoord(), EndLocation))
		return;

	StartLocation.Z += PreviewHeightOffset;
	EndLocation.Z += PreviewHeightOffset;

	if (FVector::DistSquared2D(StartLocation, EndLocation) <= KINDA_SMALL_NUMBER)
		return;

	constexpr int32 SegmentCount = 8;
	const float Height = FMath::Max(0.0f, Data->Height);
	const float ThicknessScale = FMath::Max(KINDA_SMALL_NUMBER, PreviewLineThickness / PreviewMeshBaseSize);
	ATargetingPreviewActor* PreviewActorInstance = GetPreviewActor();
	const FTransform ActorTransform = PreviewActorInstance->GetActorTransform();

	for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
	{
		const float StartAlpha = static_cast<float>(SegmentIndex) / static_cast<float>(SegmentCount);
		const float EndAlpha = static_cast<float>(SegmentIndex + 1) / static_cast<float>(SegmentCount);

		FVector SegmentStart = FMath::Lerp(StartLocation, EndLocation, StartAlpha);
		FVector SegmentEnd = FMath::Lerp(StartLocation, EndLocation, EndAlpha);
		SegmentStart.Z += 4.0f * Height * StartAlpha * (1.0f - StartAlpha);
		SegmentEnd.Z += 4.0f * Height * EndAlpha * (1.0f - EndAlpha);

		const FVector LocalStart = ActorTransform.InverseTransformPosition(SegmentStart);
		const FVector LocalEnd = ActorTransform.InverseTransformPosition(SegmentEnd);
		const FVector LocalTangent = LocalEnd - LocalStart;
		USplineMeshComponent* PathMeshComponent = PreviewActorInstance->CreatePathMeshComponent();

		if (!PathMeshComponent)
			continue;

		PathMeshComponent->SetStaticMesh(StraightPreviewMesh);
		PathMeshComponent->SetForwardAxis(ESplineMeshAxis::X, false);
		PathMeshComponent->SetStartAndEnd(LocalStart, LocalTangent, LocalEnd, LocalTangent, false);
		PathMeshComponent->SetStartScale(FVector2D(ThicknessScale, 1.0f), false);
		PathMeshComponent->SetEndScale(FVector2D(ThicknessScale, 1.0f), false);

		if (ArcPreviewMaterial)
			PathMeshComponent->SetMaterial(0, ArcPreviewMaterial);

		PathMeshComponent->UpdateMesh();
	}
}

const UScriptStruct* UArcPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return FArcPathPreviewData::StaticStruct();
}
