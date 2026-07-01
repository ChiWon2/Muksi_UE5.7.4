// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MuksiTestActor.generated.h"


class UMuksiStatusEffectComponent;

UCLASS()
class MUKSI_API AMuksiTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMuksiTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMuksiStatusEffectComponent> StatusEffectComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UMuksiStatusEffectComponent* GetStatusEffectComponent() const
	{
		return StatusEffectComponent;
	}

};
