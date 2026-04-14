#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"../DialogueRow.h"
#include "DialogueTriggerActor.generated.h"

class ADialogueTriggerCreator;
class UBoxComponent;

UCLASS()
class MUKSI_API ADialogueTriggerActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName DialogueID;

	UPROPERTY(EditAnywhere)
	EDialogueTriggerType TriggerType;

private:
	UPROPERTY()
	ADialogueTriggerCreator* DialogueCreator;
	UPROPERTY()
	ACharacter* PlayerCharacter;

public:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

public:
	ADialogueTriggerActor();
public:
	void Init(FName InID, EDialogueTriggerType InType, ADialogueTriggerCreator* Creator, ACharacter* Player);
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
protected:
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
