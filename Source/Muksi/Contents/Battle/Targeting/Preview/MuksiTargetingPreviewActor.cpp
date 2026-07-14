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

	StraightPathMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StraightPathMesh"));
	StraightPathMesh->SetupAttachment(SceneRoot);
	StraightPathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StraightPathMesh->SetGenerateOverlapEvents(false);
	StraightPathMesh->SetCastShadow(false);
	StraightPathMesh->SetVisibility(false);
}

void AMuksiTargetingPreviewActor::BeginPlay()
{
	Super::BeginPlay();

	EnsureArcPathMeshes();
	HidePreview();
}

void AMuksiTargetingPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HidePreview();
	ArcPathMeshes.Empty();
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

	case EMuksiCardTargetingPathPreviewStyle::Arrow:
		UpdateArrowPathPreview();
		return;

	case EMuksiCardTargetingPathPreviewStyle::Arc:
		UpdateArcPathPreview();
		return;

	case EMuksiCardTargetingPathPreviewStyle::None:
	default:
		HidePathPreview();
		return;
	}
}

void AMuksiTargetingPreviewActor::UpdateStraightPathPreview()
{
	FVector StartWorldLocation = CurrentCommand.SourceWorldLocation;
	FVector EndWorldLocation = CurrentCommand.AimWorldLocation;

	StartWorldLocation.Z += PathPreviewHeightOffset;
	EndWorldLocation.Z += PathPreviewHeightOffset;

	UpdatePathBody(StartWorldLocation, EndWorldLocation);
	HideArcPathMeshes();

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}
}

void AMuksiTargetingPreviewActor::UpdateArrowPathPreview()
{
	if (!ArrowMesh)
	{
		return;
	}

	FVector StartWorldLocation = CurrentCommand.SourceWorldLocation;
	FVector EndWorldLocation = CurrentCommand.AimWorldLocation;

	StartWorldLocation.Z += PathPreviewHeightOffset;
	EndWorldLocation.Z += PathPreviewHeightOffset;

	const FVector FullDirection = EndWorldLocation - StartWorldLocation;
	const float FullLength = FullDirection.Size();

	if (FullLength <= KINDA_SMALL_NUMBER)
	{
		HidePathPreview();
		return;
	}

	const FVector Direction = FullDirection / FullLength;
	const float VisibleArrowLength = FMath::Min(ArrowHeadLength, FullLength);
	const FVector ArrowBaseWorldLocation = EndWorldLocation - Direction * VisibleArrowLength;
	const FVector ArrowCenterWorldLocation = EndWorldLocation - Direction * VisibleArrowLength * 0.5f;
	const FRotator PathRotation = Direction.Rotation();
	const float ArrowLengthScale = VisibleArrowLength / BaseArrowMeshLength;
	const float ArrowWidthScale = ArrowHeadWidth / BaseArrowMeshWidth;

	if (FullLength > VisibleArrowLength)
	{
		UpdatePathBody(StartWorldLocation, ArrowBaseWorldLocation);
	}
	else if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}

	ArrowMesh->SetWorldLocation(ArrowCenterWorldLocation);
	ArrowMesh->SetWorldRotation(PathRotation);
	ArrowMesh->SetWorldScale3D(FVector(ArrowLengthScale, ArrowWidthScale, 1.0f));
	ArrowMesh->SetHiddenInGame(false);
	ArrowMesh->SetVisibility(true, true);

	HideArcPathMeshes();
}

void AMuksiTargetingPreviewActor::UpdateArcPathPreview()
{
	EnsureArcPathMeshes();

	if (ArcPathMeshes.IsEmpty())
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

	const FVector MidWorldLocation = (StartWorldLocation + EndWorldLocation) * 0.5f;
	const FVector ControlWorldLocation = MidWorldLocation + FVector(0.0f, 0.0f, FMath::Max(0.0f, CurrentCommand.ArcHeight));

	if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}

	if (PathSpline)
	{
		PathSpline->ClearSplinePoints(false);
		PathSpline->AddSplinePoint(StartWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->AddSplinePoint(ControlWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->AddSplinePoint(EndWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->UpdateSpline();
		PathSpline->SetVisibility(false);
	}

	for (int32 SegmentIndex = 0; SegmentIndex < ArcPathMeshes.Num(); ++SegmentIndex)
	{
		const float StartAlpha = static_cast<float>(SegmentIndex) / static_cast<float>(ArcPathMeshes.Num());
		const float EndAlpha = static_cast<float>(SegmentIndex + 1) / static_cast<float>(ArcPathMeshes.Num());
		const FVector SegmentStart = GetQuadraticBezierPoint(StartWorldLocation, ControlWorldLocation, EndWorldLocation, StartAlpha);
		const FVector SegmentEnd = GetQuadraticBezierPoint(StartWorldLocation, ControlWorldLocation, EndWorldLocation, EndAlpha);

		UpdatePathSegment(ArcPathMeshes[SegmentIndex], SegmentStart, SegmentEnd, ArcSegmentLengthMultiplier);
	}
}

void AMuksiTargetingPreviewActor::UpdatePathBody(const FVector& StartWorldLocation, const FVector& EndWorldLocation)
{
	UpdatePathSegment(StraightPathMesh, StartWorldLocation, EndWorldLocation);
}

void AMuksiTargetingPreviewActor::UpdatePathSegment(UStaticMeshComponent* SegmentMesh, const FVector& StartWorldLocation, const FVector& EndWorldLocation, float LengthMultiplier)
{
	if (!SegmentMesh)
	{
		return;
	}

	const FVector PathDirection = EndWorldLocation - StartWorldLocation;
	const float PathLength = PathDirection.Size();

	if (PathLength <= KINDA_SMALL_NUMBER)
	{
		SegmentMesh->SetVisibility(false);
		return;
	}

	const FVector MidWorldLocation = (StartWorldLocation + EndWorldLocation) * 0.5f;
	const FRotator PathRotation = PathDirection.Rotation();
	const float LengthScale = PathLength * LengthMultiplier / BasePathMeshLength;
	const float WidthScale = PathWidth / BasePathMeshWidth;

	SegmentMesh->SetWorldLocation(MidWorldLocation);
	SegmentMesh->SetWorldRotation(PathRotation);
	SegmentMesh->SetWorldScale3D(FVector(LengthScale, WidthScale, 1.0f));
	SegmentMesh->SetHiddenInGame(false);
	SegmentMesh->SetVisibility(true, true);
}

void AMuksiTargetingPreviewActor::EnsureArcPathMeshes()
{
	const int32 RequiredSegmentCount = FMath::Max(2, ArcSegmentCount);

	if (ArcPathMeshes.Num() == RequiredSegmentCount)
	{
		return;
	}

	for (UStaticMeshComponent* ArcPathMesh : ArcPathMeshes)
	{
		if (ArcPathMesh)
		{
			ArcPathMesh->DestroyComponent();
		}
	}

	ArcPathMeshes.Empty();

	for (int32 SegmentIndex = 0; SegmentIndex < RequiredSegmentCount; ++SegmentIndex)
	{
		const FName ComponentName = *FString::Printf(TEXT("ArcPathMesh_%02d"), SegmentIndex);
		UStaticMeshComponent* ArcPathMesh = NewObject<UStaticMeshComponent>(this, ComponentName);

		if (!ArcPathMesh)
		{
			continue;
		}

		AddInstanceComponent(ArcPathMesh);
		ArcPathMesh->SetupAttachment(SceneRoot);
		ArcPathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ArcPathMesh->SetGenerateOverlapEvents(false);
		ArcPathMesh->SetCastShadow(false);
		ArcPathMesh->SetStaticMesh(StraightPathMesh ? StraightPathMesh->GetStaticMesh() : nullptr);
		ArcPathMesh->SetMaterial(0, StraightPathMesh ? StraightPathMesh->GetMaterial(0) : nullptr);
		ArcPathMesh->RegisterComponent();
		ArcPathMesh->SetVisibility(false);
		ArcPathMeshes.Add(ArcPathMesh);
	}
}

void AMuksiTargetingPreviewActor::HideArcPathMeshes()
{
	for (UStaticMeshComponent* ArcPathMesh : ArcPathMeshes)
	{
		if (ArcPathMesh)
		{
			ArcPathMesh->SetVisibility(false);
		}
	}
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

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}

	HideArcPathMeshes();
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
	if (Radius <= 0.0f)
	{
		return FVector::OneVector;
	}

	const float Diameter = Radius * 2.0f;
	const float UniformScale = Diameter / BasePlaneSize;

	return FVector(UniformScale, UniformScale, 1.0f);
}

FVector AMuksiTargetingPreviewActor::GetQuadraticBezierPoint(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint, float Alpha) const
{
	const float OneMinusAlpha = 1.0f - Alpha;

	return OneMinusAlpha * OneMinusAlpha * StartPoint + 2.0f * OneMinusAlpha * Alpha * ControlPoint + Alpha * Alpha * EndPoint;
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