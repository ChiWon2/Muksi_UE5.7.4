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
	
	ExchangeIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExchangeIndicator"));
	ExchangeIndicatorMesh->SetupAttachment(SceneRoot);
	
	TargetIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetIndicatorMesh"));
	TargetIndicatorMesh->SetupAttachment(SceneRoot);
}

void ABattleGridTile::BeginPlay()
{
	Super::BeginPlay();
	
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

void ABattleGridTile::SetExchangeIndicator(const EMuksiBattleCardType& IndicatorType)
{
	ExchangeIndicatorMesh->SetVisibility(true);
	
	if (!AttackableIndicatorMaterial && !MoveIndicatorMaterial && !BlockedIndicatorMaterial){UE_LOG(LogTemp, Error, TEXT("IndicatorMaterior is null (BattleGridTile.cpp)")); return;}
	
	switch (IndicatorType)
	{
		case EMuksiBattleCardType::None:
		ExchangeIndicatorMesh->SetMaterial(0, AttackableIndicatorMaterial);
		break;
		case EMuksiBattleCardType::RangeAttack:
		case EMuksiBattleCardType::Defense:
		case EMuksiBattleCardType::Heal:
		case EMuksiBattleCardType::Move:
		ExchangeIndicatorMesh->SetMaterial(0, MoveIndicatorMaterial);
		break;
		case EMuksiBattleCardType::Rush:
		ExchangeIndicatorMesh->SetMaterial(0, BlockedIndicatorMaterial);
		break;
		default:
		UE_LOG(LogTemp, Error, TEXT("[BattleGridTile] SetTargetIndicator Type Error"));
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
	
	UE_LOG(LogTemp, Log, TEXT("[ABattleGridTile]Clicked Grid Coord - Q: %d, R: %d"), GridCoord.X, GridCoord.Y);
}



