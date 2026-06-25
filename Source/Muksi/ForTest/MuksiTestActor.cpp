// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiTestActor.h"
#include"../ConditionHandle/Utils/ConditionUtils.h"
#include"../GameEventHandle/Utils/GameEventUtils.h"
// Sets default values
AMuksiTestActor::AMuksiTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMuksiTestActor::BeginPlay()
{
}

// Called every frame
void AMuksiTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

