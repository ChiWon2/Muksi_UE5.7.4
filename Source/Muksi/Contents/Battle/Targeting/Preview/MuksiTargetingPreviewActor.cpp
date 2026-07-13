#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewActor.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
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

	if (PathSpline)
	{
		PathSpline->ClearSplinePoints();
		PathSpline->SetVisibility(false);
	}

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