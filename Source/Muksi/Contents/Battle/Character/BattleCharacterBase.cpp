// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

// Sets default values
ABattleCharacterBase::ABattleCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);

	OnClicked.AddDynamic(this, &ABattleCharacterBase::HandleClicked);

}

// Called when the game starts or when spawned
void ABattleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleCharacterBase::HandleClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("BattleCharacter clicked! Actor: %s, Button: %s"),
		*GetNameSafe(TouchedActor),
		*ButtonPressed.ToString());
}


void ABattleCharacterBase::OnSelected()
{
	UE_LOG(LogTemp, Warning, TEXT("Selected"));
}

void ABattleCharacterBase::OnDeselected()
{
	UE_LOG(LogTemp, Warning, TEXT("Deselected"));
}

