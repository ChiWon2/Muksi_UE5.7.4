// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestObjectiveCreator.generated.h"

UCLASS()
class MUKSI_API AQuestObjectiveCreator : public AActor
{
    GENERATED_BODY()

public:
    AQuestObjectiveCreator();
protected:
    virtual void BeginPlay() override;
public:
    UPROPERTY(EditAnywhere, Category = "Quest")
    TMap<FName, TSubclassOf<AQuestObjectiveTarget>> ObjectiveTargetClassMap;

public:
    UFUNCTION(BlueprintCallable)
    AQuestObjectiveTarget* CreateObjectiveTarget(FName ObjectiveID,const FTransform& SpawnTransform);
};