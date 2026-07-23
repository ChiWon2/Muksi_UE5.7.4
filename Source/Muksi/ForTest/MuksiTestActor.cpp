// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiTestActor.h"
#include"../ConditionHandle/Utils/ConditionUtils.h"
#include"../GameEventHandle/Utils/GameEventUtils.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"

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

}

// Called every frame
void AMuksiTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMuksiTestActor::TestPlayAnimation()
{

	ABattleCharacterBase* BattleCharacter =Cast<ABattleCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(),ABattleCharacterBase::StaticClass()));

	if (!BattleCharacter)
	{
		return;
	}

	UMuksiBattleAnimationComponent* AnimationComponent = BattleCharacter->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AnimationComponent)
	{
		return;
	}

	AnimationComponent->PlayAnimMontage(EMuksiWeaponTypes::Fist, MuksiBattleAnimationKeys::Attack_1);
}