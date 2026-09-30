#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
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
	SelectionPreviewMesh->SetAffectDynamicIndirectLighting(false);
	SelectionPreviewMesh->SetAffectDistanceFieldLighting(false);
	SelectionPreviewMesh->SetVisibleInRayTracing(false);
	SelectionPreviewMesh->SetCanEverAffectNavigation(false);
	SelectionPreviewMesh->SetVisibility(false);

	AreaPreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaPreviewMesh"));
	AreaPreviewMesh->SetMobility(EComponentMobility::Movable);
	AreaPreviewMesh->SetupAttachment(SceneRoot);
	AreaPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaPreviewMesh->SetCastShadow(false);
	AreaPreviewMesh->SetAffectDynamicIndirectLighting(false);
	AreaPreviewMesh->SetAffectDistanceFieldLighting(false);
	AreaPreviewMesh->SetVisibleInRayTracing(false);
	AreaPreviewMesh->SetCanEverAffectNavigation(false);
	AreaPreviewMesh->SetVisibility(false);
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

	for (UStaticMeshComponent* Mesh : ArrowMeshComponents)
	{
		if (!Mesh || Mesh->GetNumMaterials() <= 0)
			continue;

		if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			MID->SetVectorParameterValue(TEXT("TintColor"), Tint);
			MID->SetVectorParameterValue(TEXT("Color"), Tint);
		}
	}
}

void ATargetingPreviewActor::ClearPathPreview()
{
	ClearPathMeshComponents();
	ClearArrowMeshComponents();
}

void ATargetingPreviewActor::ClearAreaPreview()
{
	AreaPreviewMesh->SetVisibility(false);
}

void ATargetingPreviewActor::ClearAllPreview()
{
	SelectionPreviewMesh->SetVisibility(false);
	AreaPreviewMesh->SetVisibility(false);
	ClearPathMeshComponents();
	ClearArrowMeshComponents();
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
	PathMeshComponent->SetAffectDynamicIndirectLighting(false);
	PathMeshComponent->SetAffectDistanceFieldLighting(false);
	PathMeshComponent->SetVisibleInRayTracing(false);
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


UStaticMeshComponent* ATargetingPreviewActor::CreateArrowMeshComponent()
{
	UStaticMeshComponent* ArrowMeshComponent = NewObject<UStaticMeshComponent>(this);

	if (!ArrowMeshComponent)
		return nullptr;

	ArrowMeshComponent->SetMobility(EComponentMobility::Movable);
	ArrowMeshComponent->SetupAttachment(SceneRoot);
	ArrowMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowMeshComponent->SetCastShadow(false);
	ArrowMeshComponent->SetAffectDynamicIndirectLighting(false);
	ArrowMeshComponent->SetAffectDistanceFieldLighting(false);
	ArrowMeshComponent->SetVisibleInRayTracing(false);
	ArrowMeshComponent->SetCanEverAffectNavigation(false);
	AddInstanceComponent(ArrowMeshComponent);
	ArrowMeshComponent->RegisterComponent();
	ArrowMeshComponents.Add(ArrowMeshComponent);
	ApplyPreviewStyle();

	return ArrowMeshComponent;
}

void ATargetingPreviewActor::ClearArrowMeshComponents()
{
	for (UStaticMeshComponent* ArrowMeshComponent : ArrowMeshComponents)
	{
		if (IsValid(ArrowMeshComponent))
			ArrowMeshComponent->DestroyComponent();
	}

	ArrowMeshComponents.Empty();
}
