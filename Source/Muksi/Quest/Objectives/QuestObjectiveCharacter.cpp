// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestObjectiveCharacter.h"

AQuestObjectiveCharacter::AQuestObjectiveCharacter()
{
	OnTakeAnyDamage.AddDynamic(this, &AQuestObjectiveCharacter::HandleDamageTaken);
}

void AQuestObjectiveCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AQuestObjectiveCharacter::HandleDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.f)
        return;

    CurrentHP -= Damage;

    if (CurrentHP <= 0.f)
    {
        NotifyObjectiveID();
        Destroy();
    }
}
