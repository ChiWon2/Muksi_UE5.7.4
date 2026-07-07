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
	
	ExchangeIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExchangeIndicator"));
	ExchangeIndicatorMesh->SetupAttachment(SceneRoot);
	
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

void ABattleGridTile::SetExchangeIndicator(int32 IndicatorType)
{
	//Include하기 귀찮아서 int로 받음
	//0 - 공격(범위)
	//1 - 이동
	//2 - 방어
	ExchangeIndicatorMesh->SetVisibility(true);
	
	if (!AttackableIndicatorMaterial && !MoveIndicatorMaterial && !BlockedIndicatorMaterial){UE_LOG(LogTemp, Error, TEXT("IndicatorMaterior is null (BattleGridTile.cpp)")); return;}
	
	switch (IndicatorType)
	{
		case 0:
		ExchangeIndicatorMesh->SetMaterial(0, AttackableIndicatorMaterial);
		break;
		case 1:
		ExchangeIndicatorMesh->SetMaterial(0, MoveIndicatorMaterial);
		break;
		case 2:
		ExchangeIndicatorMesh->SetMaterial(0, BlockedIndicatorMaterial);
		break;
		default:
		UE_LOG(LogTemp, Error, TEXT("SetTargetIndicator Type Error (BattleGridTile.cpp)"));
		break;
	}
}


void ABattleGridTile::ClearExchangeIndicator()
{
	ExchangeIndicatorMesh->SetVisibility(false);
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



