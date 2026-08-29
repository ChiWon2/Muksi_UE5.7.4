#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Materials/MaterialInstanceDynamic.h"

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
	ClearAllPreview();
	ApplyPreviewStyle();
}

void ATargetingPreviewActor::ApplyPreviewStyle()
{
	const FLinearColor Tint(0.05f, 0.45f, 1.0f, 1.0f);
	TArray<UMeshComponent*> Meshes;
	Meshes.Add(SelectionPreviewMesh);
	Meshes.Add(AreaPreviewMesh);
	Meshes.Add(ArrowPreviewMesh);
	for (UMeshComponent* Mesh : Meshes)
	{
		if (!Mesh || Mesh->GetNumMaterials() <= 0)
		{
			continue;
		}

		if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			MID->SetVectorParameterValue(TEXT("TintColor"), Tint);
			MID->SetVectorParameterValue(TEXT("Color"), Tint);
		}
	}

	for (USplineMeshComponent* Mesh : PathMeshComponents)
	{
		if (!Mesh || Mesh->GetNumMaterials() <= 0)
		{
			continue;
		}

		if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			MID->SetVectorParameterValue(TEXT("TintColor"), Tint);
			MID->SetVectorParameterValue(TEXT("Color"), Tint);
		}
	}
}

void ATargetingPreviewActor::ClearPathPreview()
{
	ArrowPreviewMesh->SetVisibility(false);
	PathSpline->ClearSplinePoints(false);
	PathSpline->UpdateSpline();
	ClearPathMeshComponents();
}

void ATargetingPreviewActor::ClearAreaPreview()
{
	AreaPreviewMesh->SetVisibility(false);
}

void ATargetingPreviewActor::ClearAllPreview()
{
	SelectionPreviewMesh->SetVisibility(false);
	AreaPreviewMesh->SetVisibility(false);
	ArrowPreviewMesh->SetVisibility(false);
	PathSpline->ClearSplinePoints(false);
	PathSpline->UpdateSpline();
	ClearPathMeshComponents();
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
	ApplyPreviewStyle();

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
