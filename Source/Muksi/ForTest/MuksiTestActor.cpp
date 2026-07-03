// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiTestActor.h"
#include"../ConditionHandle/Utils/ConditionUtils.h"
#include"../GameEventHandle/Utils/GameEventUtils.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"


// Sets default values
AMuksiTestActor::AMuksiTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StatusEffectComponent = CreateDefaultSubobject<UMuksiStatusEffectComponent>(TEXT("StatusEffectComponent"));
}

// Called when the game starts or when spawned
void AMuksiTestActor::BeginPlay()
{
    Super::BeginPlay();

    if (!StatusEffectComponent)
    {
        return;
    }

    StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Poison,3,2);

    StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Burn,2,3);
    StatusEffectComponent->AddStatusEffect(TEXT("Burn"), 2, 3);

    StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Paralysis,1,2);
    StatusEffectComponent->SubtractStatusEffect(MuksiStatusEffectIDs::Paralysis, 1, 2);
    StatusEffectComponent->RemoveStatusEffect(MuksiStatusEffectIDs::Paralysis);

    StatusEffectComponent->OnRoundStart();
}

// Called every frame
void AMuksiTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

