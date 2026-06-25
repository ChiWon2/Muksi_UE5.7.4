// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"

#include "Components/ArrowComponent.h"

// Sets default values
ABattleGridTile::ABattleGridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
	

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	
	CenterPointComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterPointComponent"));
	CenterPointComponent->SetupAttachment(SceneRoot);
	
	TargetIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("TargetIndicatorMesh")
	);
	TargetIndicatorMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void ABattleGridTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleGridTile::SetTargetIndicatorVisible(bool bVisible)
{
	if (!TargetIndicatorMesh)
	{
		return;
	}

	TargetIndicatorMesh->SetVisibility(bVisible);
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
	
	UE_LOG(LogTemp, Log, TEXT("Clicked Grid Coord - X: %d, Y: %d"),
		GridCoord.X,
		GridCoord.Y
	);
}



