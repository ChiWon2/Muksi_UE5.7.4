// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"

#include "Components/ArrowComponent.h"

ABattleGridTile::ABattleGridTile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
	
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	
	CenterPointComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterPointComponent"));
	CenterPointComponent->SetupAttachment(SceneRoot);
	
	ExtraIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExtraIndicator"));
	ExtraIndicatorMesh->SetupAttachment(SceneRoot);
	
	TargetIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetIndicatorMesh"));
	TargetIndicatorMesh->SetupAttachment(SceneRoot);
	TargetIndicatorMesh->SetForceDisableNanite(true);
}

void ABattleGridTile::BeginPlay()
{
	Super::BeginPlay();

	if (TargetIndicatorMesh)
	{
		TargetIndicatorMesh->SetVisibility(false);
	}

	if (ExtraIndicatorMesh)
	{
		ExtraIndicatorMesh->SetVisibility(false);
	}
}

void ABattleGridTile::SetTargetIndicatorVisible(bool bVisible)
{
	if (!TargetIndicatorMesh)
	{
		return;
	}

	TargetIndicatorMesh->SetTranslucentSortPriority(100);
	TargetIndicatorMesh->SetVisibility(bVisible);
	TargetIndicatorMesh->SetMaterial(0, NormalIndicatorMaterial);
}



FVector ABattleGridTile::GetGridCenterWorldLocation() const
{
	if (!CenterPointComponent){return GetActorLocation();}
	
	return CenterPointComponent->GetComponentLocation();
}

FTransform ABattleGridTile::GetCharacterSpawnTransform() const
{
	if (CenterPointComponent)
	{
		return CenterPointComponent->GetComponentTransform();
	}

	return GetActorTransform();
}

void ABattleGridTile::OnHoverBegin()
{
	
}

void ABattleGridTile::OnHoverEnd()
{
	
}

void ABattleGridTile::OnGridSelected_Implementation()
{
	ISelectGridInterface::OnGridSelected_Implementation();
	
	UE_LOG(LogTemp, Log, TEXT("[ABattleGridTile]Clicked Grid Coord - Q: %d, R: %d"), GridCoord.X, GridCoord.Y);
}



