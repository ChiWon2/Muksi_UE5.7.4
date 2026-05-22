// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObjectiveTarget.h"
#include "QuestObjectiveCharacter.generated.h"

UCLASS()
class MUKSI_API AQuestObjectiveCharacter : public AQuestObjectiveTarget
{
	GENERATED_BODY()
	
public:
	AQuestObjectiveCharacter();
protected:
    UPROPERTY(EditAnywhere)
    float CurrentHP = 100.f;

    virtual void BeginPlay() override;

public:
    UFUNCTION()
    void HandleDamageTaken(AActor* DamagedActor,float Damage,const UDamageType* DamageType,AController* InstigatedBy,AActor* DamageCauser);

};
