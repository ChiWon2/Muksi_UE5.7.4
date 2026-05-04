// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueTriggerCreator.h"

#include "../DialogueSubsystem.h"
#include "../DialogueTriggerSubsystem.h"
#include "../TravelTime/TravelTimeSubsystem.h"

#include "DialogueTriggerActor.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

ADialogueTriggerCreator::ADialogueTriggerCreator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADialogueTriggerCreator::BeginPlay()
{
	Super::BeginPlay();

	TravelTimeSubsystem = UTravelTimeSubsystem::Get(this);
	DialogueSubsystem = UDialogueSubsystem::Get(this);
	DialogueTriggerSubsystem = UDialogueTriggerSubsystem::Get(this);

	if (TravelTimeSubsystem)
	{
		TravelTimeSubsystem->OnHourPassed.AddDynamic(this,&ADialogueTriggerCreator::OnTravelTimeUpdated);
	}
}

void ADialogueTriggerCreator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TravelTimeSubsystem)
	{
		TravelTimeSubsystem->OnHourPassed.RemoveDynamic(this,&ADialogueTriggerCreator::OnTravelTimeUpdated);
	}

	Super::EndPlay(EndPlayReason);
}

bool ADialogueTriggerCreator::ExecuteTriggerCheck()
{
	// 테스트용 확률
	float BaseChance = 50.0f;
	float MaxChance = 100.0f;

	float Chance =
		BaseChance + ((MaxChance - BaseChance) * (TEST_Perception / 100.0f));

	float Roll = FMath::FRandRange(0.0f, 100.0f);

	return Roll <= Chance;
}

void ADialogueTriggerCreator::CreatePopUpDialogue(const FName& DialogueID,EDialogueTriggerType Type)
{
	if (!DialogueSubsystem)
	{
		UE_LOG(LogTemp, Error,TEXT("[DialogueTriggerCreator] DialogueSubsystem is null"));
		return;
	}

	DialogueSubsystem->StartDialogue(DialogueID);

	UE_LOG(LogTemp, Log,
		TEXT("[DialogueTriggerCreator] Popup Dialogue!!"));
}

void ADialogueTriggerCreator::CreateInteractableObject(const FName& DialogueID,EDialogueTriggerType Type)
{
	if (!DialogueTriggerActorClass)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerCreator] DialogueTriggerActorClass is null"));
		return;
	}

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSys)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerCreator] Failed to find NavSystem"));
		return;
	}

	// Creator Actor 위치 기준
	const FVector OriginLocation = GetActorLocation();

	// 랜덤 방향
	FVector RandomDir = FMath::VRand();
	RandomDir.Z = 0.f;
	RandomDir.Normalize();

	// 랜덤 거리
	const float Distance =FMath::RandRange(300.f, 500.f);

	const FVector DesiredLocation =
		OriginLocation + (RandomDir * Distance);

	// NavMesh 보정
	FNavLocation ProjectedLocation;

	const bool bProjected =
		NavSys->ProjectPointToNavigation(
			DesiredLocation,
			ProjectedLocation,
			FVector(150.f, 150.f, 300.f)
		);

	if (!bProjected)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerCreator] Failed to project to NavMesh"));
		return;
	}

	// Spawn
	FActorSpawnParameters Params;

	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	CreatedTriggerActor =
		GetWorld()->SpawnActor<ADialogueTriggerActor>(
			DialogueTriggerActorClass,
			ProjectedLocation.Location,
			FRotator::ZeroRotator,
			Params
		);

	if (!CreatedTriggerActor)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DialogueTriggerCreator] Failed to spawn trigger actor"));
		return;
	}

	CreatedTriggerActor->Init(
		DialogueID,
		Type,
		this
	);

	UE_LOG(LogTemp, Log,
		TEXT("[DialogueTriggerCreator] Spawned Interactable Trigger Actor!! TriggerID : %s"),
		*DialogueID.ToString());
}

EDialogueTriggerType ADialogueTriggerCreator::SelectTriggerType()
{
	const float SingleChance = 0.8f;
	const float ReusableChance = 1.f - SingleChance;

	const float Rand = FMath::FRand();

	const bool bHasSingle =
		!DialogueTriggerSubsystem->IsTriggerIDsEmpty(
			EDialogueTriggerType::Single
		);

	EDialogueTriggerType Result;

	if (bHasSingle && Rand <= SingleChance)
	{
		Result = EDialogueTriggerType::Single;
	}
	else
	{
		Result = EDialogueTriggerType::Reusable;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[DialogueTriggerCreator] TriggerType Picked | Single: %.0f%% Reusable: %.0f%% | Rand: %.2f | Result: %s"),
		SingleChance * 100.f,
		ReusableChance * 100.f,
		Rand,
		*UEnum::GetValueAsString(Result));

	return Result;
}

EDialogueCreateType ADialogueTriggerCreator::SelectCreateType()
{
	const float PopUpChance = 1.f;
	const float InteractableChance = 1.f - PopUpChance;

	const float Rand = FMath::FRand();

	const EDialogueCreateType Result =
		(Rand <= PopUpChance)
		? EDialogueCreateType::PopUp
		: EDialogueCreateType::InteractableObject;

	return Result;
}

void ADialogueTriggerCreator::CreateRandomDialogue()
{
	if (!DialogueSubsystem)
		return;

	// 핵심: Dialogue 진행 중이면 생성 자체를 막음
	if (DialogueSubsystem->IsDialogueActive())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[DialogueTriggerCreator] Dialogue Active - Skip Trigger Generation"));
		return;
	}

	// 이미 생성된 Trigger 있으면 중복 생성 방지
	if (CreatedTriggerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerCreator] Already Has CreatedTriggerActor!!"));
		return;
	}

	EDialogueTriggerType TriggerType = SelectTriggerType();
	FName DialogueID = DialogueTriggerSubsystem->ExtractRandomTriggerID(TriggerType);

	if (DialogueID.IsNone())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[DialogueTriggerCreator] No Valid DialogueID Found"));
		return;
	}

	EDialogueCreateType CreateType = SelectCreateType();

	switch (CreateType)
	{
	case EDialogueCreateType::PopUp:
		CreatePopUpDialogue(DialogueID, TriggerType);
		break;

	case EDialogueCreateType::InteractableObject:
		CreateInteractableObject(DialogueID, TriggerType);
		break;
	}
}
void ADialogueTriggerCreator::OnTravelTimeUpdated(FTravelDate NewTime)
{
	if (CreatedTriggerActor)
	{
		UE_LOG(LogTemp, Log,
			TEXT("[ADialogueTriggerCreator] Trigger object already exists"));
		return;
	}

	++TimeAccumulator;

	if (TimeAccumulator >= CheckHour)
	{
		TimeAccumulator = 0;

		if (ExecuteTriggerCheck())
		{
			CreateRandomDialogue();
		}
	}

	UE_LOG(LogTemp, Log,
		TEXT("[ADialogueTriggerCreator] OnTravelTimeUpdated Called"));
}