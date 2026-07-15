#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewActor.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingPreviewRenderer.h"

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

	CircleWorldAreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CircleWorldAreaMesh"));
	CircleWorldAreaMesh->SetupAttachment(SceneRoot);
	CircleWorldAreaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CircleWorldAreaMesh->SetGenerateOverlapEvents(false);
	CircleWorldAreaMesh->SetCastShadow(false);
	CircleWorldAreaMesh->SetVisibility(false);

	ConeWorldAreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeWorldAreaMesh"));
	ConeWorldAreaMesh->SetupAttachment(SceneRoot);
	ConeWorldAreaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConeWorldAreaMesh->SetGenerateOverlapEvents(false);
	ConeWorldAreaMesh->SetCastShadow(false);
	ConeWorldAreaMesh->SetVisibility(false);

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

	PreviewRenderer = NewObject<UMuksiTargetingPreviewRenderer>(this);

	if (PreviewRenderer)
	{
		PreviewRenderer->Initialize(this, SceneRoot, GridManager, RangePreviewMesh, CircleWorldAreaMesh, ConeWorldAreaMesh, StraightPathMesh, ArrowMesh, PathSpline);
	}

	HidePreview();
}

void AMuksiTargetingPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PreviewRenderer)
	{
		PreviewRenderer->Hide();
	}

	PreviewRenderer = nullptr;
	GridManager = nullptr;

	Super::EndPlay(EndPlayReason);
}

void AMuksiTargetingPreviewActor::InitializePreviewActor(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;

	if (PreviewRenderer)
	{
		PreviewRenderer->SetGridManager(GridManager);
	}

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

	if (PreviewRenderer)
	{
		PreviewRenderer->Update(CurrentCommand, MakePreviewSettings());
	}
}

void AMuksiTargetingPreviewActor::HidePreview()
{
	CurrentCommand.Reset();

	if (PreviewRenderer)
	{
		PreviewRenderer->Hide();
	}
	else
	{
		RangePreviewMesh->SetVisibility(false);
		CircleWorldAreaMesh->SetVisibility(false);
		ConeWorldAreaMesh->SetVisibility(false);
		StraightPathMesh->SetVisibility(false);
		ArrowMesh->SetVisibility(false);
	}

	SetActorHiddenInGame(true);
}

FMuksiTargetingPreviewSettings AMuksiTargetingPreviewActor::MakePreviewSettings() const
{
	FMuksiTargetingPreviewSettings Settings;

	Settings.SelectionRangeSettings.HeightOffset = RangePreviewHeightOffset;
	Settings.SelectionRangeSettings.bIncludeOuterTileRadius = bIncludeOuterTileRadius;
	Settings.WorldAreaSettings.HeightOffset = WorldAreaPreviewHeightOffset;
	Settings.PathSettings.PathPreviewHeightOffset = PathPreviewHeightOffset;
	Settings.PathSettings.PathWidth = PathWidth;
	Settings.PathSettings.ArrowHeadLength = ArrowHeadLength;
	Settings.PathSettings.ArrowHeadWidth = ArrowHeadWidth;
	Settings.PathSettings.ArcSegmentCount = ArcSegmentCount;
	Settings.PathSettings.ArcSegmentLengthMultiplier = ArcSegmentLengthMultiplier;

	return Settings;
}