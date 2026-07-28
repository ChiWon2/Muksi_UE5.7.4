// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CharacterCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UCharacterCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Camera"
	)
	float ArmLength = 0.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Camera"
	)
	float FocalLength = 35.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Camera"
	)
	FVector SocketOffset = FVector::ZeroVector;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Camera"
	)
	FRotator ArmRotation = FRotator::ZeroRotator;	
};
