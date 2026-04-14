// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"../DialogueRow.h"
#include"../../TravelTime/TravelTimeSubsystem.h"
#include "DialogueTriggerCreator.generated.h"

class UDialogueSubsystem;
class UDialogueTriggerSubsystem;
class ADialogueTriggerActor;
class UTravelTimeSubsystem;

UENUM(BlueprintType)
enum class EDialogueCreateType : uint8
{
	PopUp      UMETA(DisplayName = "PopUp"),
	InteractableObject  UMETA(DisplayName = "InteractableObject"),
};

UCLASS()
class MUKSI_API ADialogueTriggerCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADialogueTriggerCreator();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckHour = 5; // TravelTimeSubsystem's Hour
private:
	int32 TimeAccumulator = 0;//TravelTimeSubsystem¿« Hour ¥‹¿ß

#pragma region FOR_TEST //TODO :: Please Delete This and Refactoring!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
public:
	//TODO:: Change It into Player's Perception Stat
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TEST_Perception = 50.0f; // 1~100


private:
	bool ExecuteTriggerCheck();
	void CreatePopUpDialogue(const FName& DialogueID, EDialogueTriggerType Type);
	void CreateInteractableObject(const FName& DialogueID , EDialogueTriggerType Type);
	EDialogueTriggerType SelectTriggerType();
	EDialogueCreateType SelectCreateType();
public :
	UFUNCTION(BlueprintCallable)
	void CreateRandomDialogue();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ADialogueTriggerActor> DialogueTriggerActorClass;

public:
	UPROPERTY()
	ADialogueTriggerActor* CreatedTriggerActor;


#pragma endregion
private:
	class ACharacter* PlayerCharacter;
	UDialogueSubsystem* DialogueSubsystem;
	UDialogueTriggerSubsystem* DialogueTriggerSubsystem;
	UTravelTimeSubsystem* TravelTimeSubsystem;
	
	UFUNCTION(BlueprintCallable)
	void OnTravelTimeUpdated(FTravelDate NewTime);
};
