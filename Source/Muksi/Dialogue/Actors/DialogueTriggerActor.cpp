// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTriggerActor.h"
#include "DialogueTriggerCreator.h"
#include"Components/BoxComponent.h"
#include"../DialogueSubsystem.h"
#include"../DialogueTriggerSubsystem.h"
#include"GameFramework/Character.h"

ADialogueTriggerActor::ADialogueTriggerActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Box Collider
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->SetGenerateOverlapEvents(true);
}

void ADialogueTriggerActor::Init(FName inID, EDialogueTriggerType InType, ADialogueTriggerCreator* Creator, ACharacter* Player)
{
	DialogueID = inID;
	TriggerType = InType;
	DialogueCreator = Creator;
	PlayerCharacter = Player;

	if (DialogueID == NAME_None)
		UE_LOG(LogTemp, Error, TEXT("[DialogueTrrigerActor] There is no Loaded Information"));
}

void ADialogueTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerBox)
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADialogueTriggerActor::HandleOverlap);
}

void ADialogueTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DialogueID == NAME_None)
	{
		UE_LOG(LogTemp, Error, TEXT("[DialogueTrrigerActor] There is no Loaded Information"));
		return;
	}

	const float DistSq =FVector::DistSquared(GetActorLocation(), PlayerCharacter->GetActorLocation());	//PlayerCharacter와 거리가 너무 멀어지면 like 1000 자멸하고, FName을 SubSystem 에 다시 반환한다.
	if (DistSq > FMath::Square(1000.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerActor]Destroy and Retrieve DialogueID to System!! PlayerDist : %.2f"), DistSq);
		UDialogueTriggerSubsystem* DialogueTriggerSubsystem = UDialogueTriggerSubsystem::Get(GetWorld());
		DialogueTriggerSubsystem->RetrieveTriggerID(DialogueID, TriggerType);
		DialogueCreator->CreatedTriggerActor = nullptr;
		Destroy();
	}
}

void ADialogueTriggerActor::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor != Cast<AActor>(PlayerCharacter))
		return;
	UDialogueSubsystem* DialogueSubsystem = UDialogueSubsystem::Get(GetWorld());
	UE_LOG(LogTemp, Warning,TEXT("[DialogueTriggerActor] Player Overlap Trigger : %s"),*DialogueID.ToString());
	DialogueSubsystem->StartDialogue(DialogueID);
	//TODO :: Need Return To Pool System
	DialogueCreator->CreatedTriggerActor = nullptr;
	Destroy();
	
}

