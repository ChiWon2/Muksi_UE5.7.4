// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueTriggerActor.h"

#include "DialogueTriggerCreator.h"

#include "../DialogueSubsystem.h"
#include "../DialogueTriggerSubsystem.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ADialogueTriggerActor::ADialogueTriggerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Box Collider
	TriggerBox =
		CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);

	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	TriggerBox->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Overlap
	);

	TriggerBox->SetGenerateOverlapEvents(true);
}

void ADialogueTriggerActor::Init(FName InID,EDialogueTriggerType InType,ADialogueTriggerCreator* Creator)
{
	DialogueID = InID;
	TriggerType = InType;
	DialogueCreator = Creator;

	if (DialogueID == NAME_None)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerActor] There is no loaded DialogueID"));
	}
}

void ADialogueTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this,
			&ADialogueTriggerActor::HandleOverlap
		);
	}
}

void ADialogueTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DialogueID == NAME_None)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerActor] There is no loaded DialogueID"));
		return;
	}

	if (!DialogueCreator)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerActor] DialogueCreator is null"));
		Destroy();
		return;
	}

	// Creator 기준 거리 체크
	const float DistSq =
		FVector::DistSquared(
			GetActorLocation(),
			DialogueCreator->GetActorLocation()
		);

	// 너무 멀어지면 제거 + ID 반환
	if (DistSq > FMath::Square(1000.f))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[DialogueTriggerActor] Destroy and retrieve DialogueID!! DistSq : %.2f"),
			DistSq);

		if (UDialogueTriggerSubsystem* DialogueTriggerSubsystem =
			UDialogueTriggerSubsystem::Get(GetWorld()))
		{
			DialogueTriggerSubsystem->RetrieveTriggerID(
				DialogueID,
				TriggerType
			);
		}

		DialogueCreator->CreatedTriggerActor = nullptr;

		Destroy();
	}
}

void ADialogueTriggerActor::HandleOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->IsA<ACharacter>())
	{
		return;
	}

	UDialogueSubsystem* DialogueSubsystem =
		UDialogueSubsystem::Get(GetWorld());

	if (!DialogueSubsystem)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerActor] DialogueSubsystem is null"));
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[DialogueTriggerActor] Player Overlap Trigger : %s"),
		*DialogueID.ToString());

	DialogueSubsystem->StartDialogue(DialogueID);

	if (DialogueCreator)
	{
		DialogueCreator->CreatedTriggerActor = nullptr;
	}

	// TODO :: Need Return To Pool System

	Destroy();
}