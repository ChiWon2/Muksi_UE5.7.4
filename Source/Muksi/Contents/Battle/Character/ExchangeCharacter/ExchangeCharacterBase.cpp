// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/ExchangeCharacter/ExchangeCharacterBase.h"

// Sets default values
AExchangeCharacterBase::AExchangeCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("MeshComponent")
	);
	MeshComponent->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AExchangeCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExchangeCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

