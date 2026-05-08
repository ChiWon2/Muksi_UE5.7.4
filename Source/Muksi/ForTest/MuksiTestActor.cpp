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
	Super::BeginPlay();
	if (ConditionUtils::CheckTravelTime(this, FTravelDate(1857, 3, 2, 14), EConditionCompareOp::Less))
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiTestActor]TravelTime Check Complete"));
	}
	FName TestQuestID = FName("questID111");
	GameEventUtils::ExecuteAcceptQuest(this, TestQuestID);
}

// Called every frame
void AMuksiTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

