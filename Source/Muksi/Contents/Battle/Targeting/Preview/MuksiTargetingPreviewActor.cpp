#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewActor.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

AMuksiTargetingPreviewActor::AMuksiTargetingPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	RangePreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RangePreviewMesh"));
	RangePreviewMesh->SetupAttachment(SceneRoot);
	RangePreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangePreviewMesh->SetGenerateOverlapEvents(false);
	RangePreviewMesh->SetCastShadow(false);
	RangePreviewMesh->SetVisibility(false);

	WorldAreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldAreaMesh"));
	WorldAreaMesh->SetupAttachment(SceneRoot);
	WorldAreaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WorldAreaMesh->SetGenerateOverlapEvents(false);
	WorldAreaMesh->SetCastShadow(false);
	WorldAreaMesh->SetVisibility(false);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(SceneRoot);
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowMesh->SetGenerateOverlapEvents(false);
	ArrowMesh->SetCastShadow(false);
	ArrowMesh->SetVisibility(false);

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(SceneRoot);
	PathSpline->SetClosedLoop(false);
	PathSpline->SetVisibility(false);

	StraightPathMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("StraightPathMesh"));
	StraightPathMesh->SetupAttachment(SceneRoot);
	StraightPathMesh->SetForwardAxis(ESplineMeshAxis::X);
	StraightPathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StraightPathMesh->SetGenerateOverlapEvents(false);
	StraightPathMesh->SetCastShadow(false);
	StraightPathMesh->SetVisibility(false);
}

void AMuksiTargetingPreviewActor::BeginPlay()
{
	Super::BeginPlay();

	HidePreview();
}

void AMuksiTargetingPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HidePreview();
	GridManager = nullptr;

	Super::EndPlay(EndPlayReason);
}

void AMuksiTargetingPreviewActor::InitializePreviewActor(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
	HidePreview();
}

void AMuksiTargetingPreviewActor::UpdatePreview(const FMuksiTargetingPreviewCommand& Command)
{
	CurrentCommand = Command;

	if (!CurrentCommand.bVisible)
	{
		HidePreview();
		return;
	}

	SetActorHiddenInGame(false);
	UpdateSelectionRangePreview();
	UpdateWorldAreaPreview();
	UpdatePathPreview();
	UpdateGridPreview();
}

void AMuksiTargetingPreviewActor::HidePreview()
{
	CurrentCommand.Reset();

	if (RangePreviewMesh)
	{
		RangePreviewMesh->SetVisibility(false);
	}

	if (WorldAreaMesh)
	{
		WorldAreaMesh->SetVisibility(false);
	}

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}

	HidePathPreview();

	if (GridManager)
	{
		GridManager->AllClearGridHovered();
	}

	SetActorHiddenInGame(true);
}

void AMuksiTargetingPreviewActor::UpdateSelectionRangePreview()
{
	if (!RangePreviewMesh)
	{
		return;
	}

	if (!GridManager)
	{
		RangePreviewMesh->SetVisibility(false);
		return;
	}

	if (!CurrentCommand.bShowSelectionRange || CurrentCommand.SelectionRange <= 0)
	{
		RangePreviewMesh->SetVisibility(false);
		return;
	}

	const float RangeWorldRadius = GetSelectionRangeWorldRadius();

	if (RangeWorldRadius <= 0.0f)
	{
		RangePreviewMesh->SetVisibility(false);
		return;
	}

	FVector PreviewLocation = CurrentCommand.SourceWorldLocation;
	PreviewLocation.Z += RangePreviewHeightOffset;

	RangePreviewMesh->SetWorldLocation(PreviewLocation);
	RangePreviewMesh->SetWorldRotation(FRotator::ZeroRotator);
	RangePreviewMesh->SetWorldScale3D(GetPlaneScaleByRadius(RangeWorldRadius));
	RangePreviewMesh->SetVisibility(true);
}

void AMuksiTargetingPreviewActor::UpdateWorldAreaPreview()
{
	if (!WorldAreaMesh)
	{
		return;
	}

	if (CurrentCommand.WorldAreaStyle == EMuksiCardTargetingWorldAreaPreviewStyle::None)
	{
		WorldAreaMesh->SetVisibility(false);
		return;
	}

	if (CurrentCommand.WorldAreaStyle != EMuksiCardTargetingWorldAreaPreviewStyle::Circle)
	{
		WorldAreaMesh->SetVisibility(false);
		return;
	}

	if (CurrentCommand.WorldAreaRadius <= 0.0f)
	{
		WorldAreaMesh->SetVisibility(false);
		return;
	}

	FVector PreviewLocation = CurrentCommand.AimWorldLocation;
	PreviewLocation.Z += WorldAreaPreviewHeightOffset;

	WorldAreaMesh->SetWorldLocation(PreviewLocation);
	WorldAreaMesh->SetWorldRotation(FRotator::ZeroRotator);
	WorldAreaMesh->SetWorldScale3D(GetPlaneScaleByRadius(CurrentCommand.WorldAreaRadius));
	WorldAreaMesh->SetVisibility(true);
}

void AMuksiTargetingPreviewActor::UpdatePathPreview()
{
	switch (CurrentCommand.PathStyle)
	{
	case EMuksiCardTargetingPathPreviewStyle::Straight:
		UpdateStraightPathPreview();
		return;

	case EMuksiCardTargetingPathPreviewStyle::None:
	case EMuksiCardTargetingPathPreviewStyle::Arc:
	case EMuksiCardTargetingPathPreviewStyle::Arrow:
	default:
		HidePathPreview();
		return;
	}
}

void AMuksiTargetingPreviewActor::UpdateStraightPathPreview()
{
	if (!PathSpline || !StraightPathMesh)
	{
		return;
	}

	FVector StartWorldLocation = CurrentCommand.SourceWorldLocation;
	FVector EndWorldLocation = CurrentCommand.AimWorldLocation;

	StartWorldLocation.Z += PathPreviewHeightOffset;
	EndWorldLocation.Z += PathPreviewHeightOffset;

	if (FVector::DistSquared(StartWorldLocation, EndWorldLocation) <= KINDA_SMALL_NUMBER)
	{
		HidePathPreview();
		return;
	}

	const FTransform ActorTransform = GetActorTransform();
	const FVector StartLocalLocation = ActorTransform.InverseTransformPosition(StartWorldLocation);
	const FVector EndLocalLocation = ActorTransform.InverseTransformPosition(EndWorldLocation);

	PathSpline->ClearSplinePoints(false);
	PathSpline->AddSplinePoint(StartLocalLocation, ESplineCoordinateSpace::Local, false);
	PathSpline->AddSplinePoint(EndLocalLocation, ESplineCoordinateSpace::Local, false);
	PathSpline->SetSplinePointType(0, ESplinePointType::Linear, false);
	PathSpline->SetSplinePointType(1, ESplinePointType::Linear, false);
	PathSpline->UpdateSpline();

	const FVector StartPosition = PathSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	const FVector EndPosition = PathSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	const FVector StraightTangent = EndPosition - StartPosition;
	const float WidthScale = StraightPathWidth / BasePathMeshSize;
	const float ThicknessScale = StraightPathThickness / BasePathMeshSize;
	const FVector2D PathScale(WidthScale, ThicknessScale);

	StraightPathMesh->SetStartAndEnd(StartPosition, StraightTangent, EndPosition, StraightTangent, true);
	StraightPathMesh->SetStartScale(PathScale, true);
	StraightPathMesh->SetEndScale(PathScale, true);
	StraightPathMesh->SetVisibility(true);
	PathSpline->SetVisibility(true);
}

void AMuksiTargetingPreviewActor::HidePathPreview()
{
	if (PathSpline)
	{
		PathSpline->ClearSplinePoints();
		PathSpline->SetVisibility(false);
	}

	if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}
}

void AMuksiTargetingPreviewActor::UpdateGridPreview()
{
	if (!GridManager)
	{
		return;
	}

	GridManager->AllClearGridHovered();

	switch (CurrentCommand.GridStyle)
	{
	case EMuksiCardTargetingGridPreviewStyle::None:
		return;

	case EMuksiCardTargetingGridPreviewStyle::AffectedTiles:
		GridManager->SetGridHovered(CurrentCommand.AffectedCoords);
		return;

	case EMuksiCardTargetingGridPreviewStyle::PathTiles:
		GridManager->SetGridHovered(CurrentCommand.PathCoords);
		return;

	case EMuksiCardTargetingGridPreviewStyle::AffectedAndPathTiles:
		GridManager->SetGridHovered(CombineCoords(CurrentCommand.AffectedCoords, CurrentCommand.PathCoords));
		return;

	default:
		return;
	}
}

float AMuksiTargetingPreviewActor::GetSelectionRangeWorldRadius() const
{
	if (!GridManager)
	{
		return 0.0f;
	}

	const float HexCenterDistance = GridManager->HexRadius * FMath::Sqrt(3.0f);
	float RangeWorldRadius = HexCenterDistance * CurrentCommand.SelectionRange;

	if (bIncludeOuterTileRadius)
	{
		RangeWorldRadius += GridManager->HexRadius;
	}

	return RangeWorldRadius;
}

FVector AMuksiTargetingPreviewActor::GetPlaneScaleByRadius(float Radius) const
{
	if (BasePlaneSize <= 0.0f)
	{
		return FVector::OneVector;
	}

	const float Diameter = Radius * 2.0f;
	const float UniformScale = Diameter / BasePlaneSize;

	return FVector(UniformScale, UniformScale, 1.0f);
}

TArray<FIntPoint> AMuksiTargetingPreviewActor::CombineCoords(const TArray<FIntPoint>& A, const TArray<FIntPoint>& B) const
{
	TArray<FIntPoint> Result = A;

	for (const FIntPoint& Coord : B)
	{
		Result.AddUnique(Coord);
	}

	return Result;
}