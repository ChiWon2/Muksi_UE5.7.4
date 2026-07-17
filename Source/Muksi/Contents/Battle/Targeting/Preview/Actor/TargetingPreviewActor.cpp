#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/DeveloperSettings/TargetingDeveloperSettings.h"

ATargetingPreviewActor::ATargetingPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(SceneRoot);

	SelectionPreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionPreviewMesh"));
	SelectionPreviewMesh->SetMobility(EComponentMobility::Movable);
	SelectionPreviewMesh->SetupAttachment(SceneRoot);
	SelectionPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionPreviewMesh->SetCastShadow(false);
	SelectionPreviewMesh->SetCanEverAffectNavigation(false);
	SelectionPreviewMesh->SetVisibility(false);

	AreaPreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaPreviewMesh"));
	AreaPreviewMesh->SetMobility(EComponentMobility::Movable);
	AreaPreviewMesh->SetupAttachment(SceneRoot);
	AreaPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaPreviewMesh->SetCastShadow(false);
	AreaPreviewMesh->SetCanEverAffectNavigation(false);
	AreaPreviewMesh->SetVisibility(false);

	ArrowPreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowPreviewMesh"));
	ArrowPreviewMesh->SetMobility(EComponentMobility::Movable);
	ArrowPreviewMesh->SetupAttachment(SceneRoot);
	ArrowPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowPreviewMesh->SetCastShadow(false);
	ArrowPreviewMesh->SetCanEverAffectNavigation(false);
	ArrowPreviewMesh->SetVisibility(false);

	GridPreviewMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GridPreviewMesh"));
	GridPreviewMesh->SetMobility(EComponentMobility::Movable);
	GridPreviewMesh->SetupAttachment(SceneRoot);
	GridPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GridPreviewMesh->SetCastShadow(false);
	GridPreviewMesh->SetCanEverAffectNavigation(false);
	GridPreviewMesh->SetVisibility(false);

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetMobility(EComponentMobility::Movable);
	PathSpline->SetupAttachment(SceneRoot);
	PathSpline->SetClosedLoop(false);
}

void ATargetingPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAllPreview();
	Super::EndPlay(EndPlayReason);
}

void ATargetingPreviewActor::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
	LoadPreviewAssets();
	ClearAllPreview();
}

void ATargetingPreviewActor::SetSelectionGridCoords(const TArray<FIntPoint>& InCoords)
{
	SelectionGridCoords = InCoords;
	RebuildGridPreview();
}

void ATargetingPreviewActor::SetPathGridCoords(const TArray<FIntPoint>& InCoords)
{
	PathGridCoords = InCoords;
	RebuildGridPreview();
}

void ATargetingPreviewActor::SetAreaGridCoords(const TArray<FIntPoint>& InCoords)
{
	AreaGridCoords = InCoords;
	RebuildGridPreview();
}

void ATargetingPreviewActor::SetGridPreviewMode(ETargetingGridPreviewMode InGridPreviewMode)
{
	GridPreviewMode = InGridPreviewMode;
	RebuildGridPreview();
}

void ATargetingPreviewActor::ClearSelectionPreview()
{
	SelectionGridCoords.Empty();
	SelectionPreviewMesh->SetVisibility(false);
	RebuildGridPreview();
}

void ATargetingPreviewActor::ClearPathPreview()
{
	PathGridCoords.Empty();
	ArrowPreviewMesh->SetVisibility(false);
	PathSpline->ClearSplinePoints(false);
	PathSpline->UpdateSpline();
	ClearPathMeshComponents();
	RebuildGridPreview();
}

void ATargetingPreviewActor::ClearAreaPreview()
{
	AreaGridCoords.Empty();
	AreaPreviewMesh->SetVisibility(false);
	RebuildGridPreview();
}

void ATargetingPreviewActor::ClearAllPreview()
{
	SelectionGridCoords.Empty();
	PathGridCoords.Empty();
	AreaGridCoords.Empty();
	SelectionPreviewMesh->SetVisibility(false);
	AreaPreviewMesh->SetVisibility(false);
	ArrowPreviewMesh->SetVisibility(false);
	PathSpline->ClearSplinePoints(false);
	PathSpline->UpdateSpline();
	ClearPathMeshComponents();
	GridPreviewMesh->ClearInstances();
	GridPreviewMesh->SetVisibility(false);
}

USplineMeshComponent* ATargetingPreviewActor::CreatePathMeshComponent()
{
	USplineMeshComponent* PathMeshComponent = NewObject<USplineMeshComponent>(this);

	if (!PathMeshComponent)
	{
		return nullptr;
	}

	PathMeshComponent->SetMobility(EComponentMobility::Movable);
	PathMeshComponent->SetupAttachment(SceneRoot);
	PathMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PathMeshComponent->SetCastShadow(false);
	PathMeshComponent->SetCanEverAffectNavigation(false);
	AddInstanceComponent(PathMeshComponent);
	PathMeshComponent->RegisterComponent();
	PathMeshComponents.Add(PathMeshComponent);

	return PathMeshComponent;
}

void ATargetingPreviewActor::ClearPathMeshComponents()
{
	for (USplineMeshComponent* PathMeshComponent : PathMeshComponents)
	{
		if (IsValid(PathMeshComponent))
		{
			PathMeshComponent->DestroyComponent();
		}
	}

	PathMeshComponents.Empty();
}

void ATargetingPreviewActor::LoadPreviewAssets()
{
	const UTargetingDeveloperSettings* Settings = GetDefault<UTargetingDeveloperSettings>();

	if (!Settings)
	{
		return;
	}

	PreviewHeightOffset = Settings->PreviewHeightOffset;

	if (UStaticMesh* GridMesh = Settings->GridPreviewMesh.LoadSynchronous())
	{
		GridPreviewMesh->SetStaticMesh(GridMesh);
	}

	if (UMaterialInterface* GridMaterial = Settings->GridPreviewMaterial.LoadSynchronous())
	{
		GridPreviewMesh->SetMaterial(0, GridMaterial);
	}
}

void ATargetingPreviewActor::RebuildGridPreview()
{
	GridPreviewMesh->ClearInstances();
	GridPreviewMesh->SetVisibility(false);

	if (!IsValid(GridManager) || !GridPreviewMesh->GetStaticMesh())
	{
		return;
	}

	TSet<FIntPoint> PreviewCoords;
	AddGridCoords(PreviewCoords, SelectionGridCoords);

	if (GridPreviewMode == ETargetingGridPreviewMode::PathTiles || GridPreviewMode == ETargetingGridPreviewMode::AffectedAndPathTiles)
	{
		AddGridCoords(PreviewCoords, PathGridCoords);
	}

	if (GridPreviewMode == ETargetingGridPreviewMode::AffectedTiles || GridPreviewMode == ETargetingGridPreviewMode::AffectedAndPathTiles)
	{
		AddGridCoords(PreviewCoords, AreaGridCoords);
	}

	for (const FIntPoint& Coord : PreviewCoords)
	{
		const FBattleGridCell* Cell = GridManager->GetCell(Coord);

		if (!Cell)
		{
			continue;
		}

		const FVector InstanceLocation = Cell->WorldLocation + FVector(0.0f, 0.0f, PreviewHeightOffset);
		const FTransform InstanceTransform(FRotator::ZeroRotator, InstanceLocation);
		GridPreviewMesh->AddInstance(InstanceTransform, true);
	}

	GridPreviewMesh->MarkRenderStateDirty();
	GridPreviewMesh->SetVisibility(GridPreviewMesh->GetInstanceCount() > 0);
}

void ATargetingPreviewActor::AddGridCoords(TSet<FIntPoint>& InOutCoords, const TArray<FIntPoint>& InCoords) const
{
	for (const FIntPoint& Coord : InCoords)
	{
		if (IsValid(GridManager) && GridManager->IsValidGridCoord(Coord))
		{
			InOutCoords.Add(Coord);
		}
	}
}