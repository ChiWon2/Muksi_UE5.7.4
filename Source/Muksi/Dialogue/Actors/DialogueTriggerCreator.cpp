// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueTriggerCreator.h"
#include"../DialogueSubsystem.h"
#include"../DialogueTriggerSubsystem.h"
#include"DialogueTriggerActor.h"
#include"Kismet/GameplayStatics.h"
#include"NavigationSystem.h"
#include"NavigationPath.h"
#include "GameFramework/Character.h"
#include"../TravelTime/TravelTimeSubsystem.h"

ADialogueTriggerCreator::ADialogueTriggerCreator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADialogueTriggerCreator::BeginPlay()
{
	Super::BeginPlay();
	if (!GetGameInstance())
		return;

	UWorld* TempWorld = GetWorld();
	if (TempWorld)
	{
		DialogueSubsystem = UDialogueSubsystem::Get(TempWorld);
		DialogueTriggerSubsystem = UDialogueTriggerSubsystem::Get(TempWorld);
		TravelTimeSubsystem = UTravelTimeSubsystem::Get(TempWorld);
	}

	TravelTimeSubsystem->OnHourPassed.AddDynamic(
		this,
		&ADialogueTriggerCreator::OnTravelTimeUpdated
	);

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ADialogueTriggerCreator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TravelTimeSubsystem)
	{
		TravelTimeSubsystem->OnHourPassed.RemoveDynamic(
			this,
			&ADialogueTriggerCreator::OnTravelTimeUpdated
		);
	}

	Super::EndPlay(EndPlayReason);
}

bool ADialogueTriggerCreator::ExecuteTriggerCheck()
{
	// Perception 기반 확률 계산
	// 기본 10%, 100이면 20%까지 선형 증가 지금은 테스트용도로 이거 바꿔야됨 TODO:: 이거 바꾸기
	float BaseChance = 50.0f;
	float MaxChance = 100.0f;
	float Chance = BaseChance + ((MaxChance - BaseChance) * (TEST_Perception / 100.0f));

	float Roll = FMath::FRandRange(0.0f, 100.0f);
	if (Roll <= Chance)
	{
		// 확률 통과! Dialogue가 생성된다.
		//UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerCreator]Trigger Check Passed! Roll: %.2f <= Chance: %.2f"), Roll, Chance);
		return true;
	}
	else
	{
		//확률 실패! 아무일도 벌어지지 않았다.
		//UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerCreator]Trigger Check Failed. Roll: %.2f > Chance: %.2f"), Roll, Chance);
	}
	return false;
}

void ADialogueTriggerCreator::CreatePopUpDialogue(const FName& DialogueID, EDialogueTriggerType Type)
{
	DialogueSubsystem->StartDialogue(DialogueID);
	UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerCreator]Popup Dialogue!!"));
}

void ADialogueTriggerCreator::CreateInteractableObject(const FName& DialogueID ,EDialogueTriggerType Type)
{
	//플레이어 캐릭터 주변 위치 약간 도끼를 휘두르면 도끼날의 위치에만 ADialogueTriggerActor 생성 , 그리고 이거 Nav로 갈수 있어야한다. 
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[DialogueTriggerCreator] Failed to Find PlayerCharacter"));
		PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("[DialogueTriggerCreator] Failed to Find NavSystem"));
		return;
	}

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	// 1. 방향 + 거리 (300~500)
	FVector RandomDir = FMath::VRand();
	RandomDir.Z = 0.f;               // 지면 기준으로 제한 (선택)
	RandomDir.Normalize();

	const float Distance = FMath::RandRange(300.f, 500.f);
	const FVector DesiredLocation = PlayerLocation + RandomDir * Distance;

	// 2. NavMesh로 보정
	FNavLocation ProjectedLocation;
	bool bProjected = NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation, FVector(150.f, 150.f, 300.f)); // 탐색 허용 범위

	if (!bProjected)
	{
		UE_LOG(LogTemp, Error,TEXT("[DialogueTriggerCreator] Failed to project location to NavMesh"));
		return;
	}

    // Spawn Actor
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	CreatedTriggerActor = GetWorld()->SpawnActor<ADialogueTriggerActor>(DialogueTriggerActorClass, ProjectedLocation, FRotator::ZeroRotator, Params);

    if (CreatedTriggerActor)
    {
		CreatedTriggerActor->Init(DialogueID, Type, this ,PlayerCharacter);
        UE_LOG(LogTemp, Log, TEXT("[DialogueTriggerCreator]Spawned Interactable Trigger Actor!!  TriggerID : %s"), *DialogueID.ToString());
    }
}

EDialogueTriggerType ADialogueTriggerCreator::SelectTriggerType()
{
	// 테스트용 확률 설정
	const float SingleChance = 0.8;     // 0~1 40% 한번만 생성되는 다이얼로그
	const float ReusableChance = 1 - SingleChance;   // 60% 반복 생성되는 다이얼로그

	const float Rand = FMath::FRand();
	const bool bHasSingle =!DialogueTriggerSubsystem->IsTriggerIDsEmpty(EDialogueTriggerType::Single);

	EDialogueTriggerType Result;

	if (bHasSingle && Rand <= SingleChance)
	{
		Result = EDialogueTriggerType::Single;
	}
	else
	{
		Result = EDialogueTriggerType::Reusable;
	}

	UE_LOG(LogTemp, Warning,TEXT("[DialogueTriggerCreator] TriggerType Picked | Single: %.0f%% Reusable: %.0f%% | Rand: %.2f | Result: %s"),SingleChance * 100.f,ReusableChance * 100.f,Rand,*UEnum::GetValueAsString(Result));

	return Result;
}

EDialogueCreateType ADialogueTriggerCreator::SelectCreateType()
{
	const float PopUpChance = 0.5f;          // 50% UI팝업 되는 다이얼로그
	const float InteractableChance = 1- PopUpChance;   // 50% 상호작용 객체 생성 다이얼로그

	const float Rand = FMath::FRand();

	const EDialogueCreateType Result =(Rand <= PopUpChance)? EDialogueCreateType::PopUp : EDialogueCreateType::InteractableObject;

	//UE_LOG(LogTemp, Log,TEXT("[DialogueTriggerCreator] CreateType Picked | PopUp: %.0f%% Interactable: %.0f%% | Rand: %.2f | Result: %s"),PopUpChance * 100.f,InteractableChance * 100.f,Rand,*UEnum::GetValueAsString(Result));

	return Result;
}

void ADialogueTriggerCreator::CreateRandomDialogue()
{
	//이미 생성된 기연지가 있으면 더 생성하지 않음.
	if (CreatedTriggerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DialogueTriggerCreator] Already Has CreatedTriggerActor!!"));
		return;
	}

	EDialogueTriggerType TriggerType = SelectTriggerType();
	FName DialogueID = DialogueTriggerSubsystem->ExtractRandomTriggerID(TriggerType);
	EDialogueCreateType CreateType = SelectCreateType();

	switch (CreateType)
	{
	case EDialogueCreateType::PopUp:
		CreatePopUpDialogue(DialogueID, TriggerType);
		break;
	case EDialogueCreateType::InteractableObject:
		CreateInteractableObject(DialogueID, TriggerType);
		break;
	default:
		break;
	}
}

void ADialogueTriggerCreator::OnTravelTimeUpdated(FTravelDate NewTime)
{
	if (CreatedTriggerActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[ADialogueTriggerCreator] : There is CreatedTriggerObject In the World, there is no ++TriggerTimeAccumulator"));
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
	UE_LOG(LogTemp, Log, TEXT("[ADialogueTriggerCreator] : OnTravelTimeUpdated Called! ++TriggerTimeAccumulator"));

}
