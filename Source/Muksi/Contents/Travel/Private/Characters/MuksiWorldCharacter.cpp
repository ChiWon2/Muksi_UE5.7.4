#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"

#include "Controllers/MuksiPlayerController.h"
#include "Controllers/PlayerMode/PlayerMode_World.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"

#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/EquipmentComponent.h"
#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"

#include "GameFramework/Controller.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

AMuksiWorldCharacter::AMuksiWorldCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = false;

	//Component
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	PlayerCurrencyComponent = CreateDefaultSubobject<UPlayerCurrencyComponent>(TEXT("PlayerCurrencyComponent"));

	WorldMapMarker = CreateDefaultSubobject<UWorldMapMarkerComponent>(TEXT("WorldMapMarker"));
	WorldMapMarker->SetupAttachment(GetCapsuleComponent());
	WorldMapMarker->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	WorldMapMarker->SortPriority = 100;
}

void AMuksiWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	WorldMapMarker->AddVisualComponent(GetMesh());

	UE_LOG(
	   LogTemp,
	   Warning,
	   TEXT(
		   "[WorldReturn][Pawn BeginPlay] Pawn=%s Controller=%s Location=%s"
	   ),
	   *GetNameSafe(this),
	   *GetNameSafe(GetController()),
	   *GetActorLocation().ToString()
   );
}

void AMuksiWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AMuksiWorldCharacter::Interact);
		}

		if (InventoryEquipmentAction)
		{
			EnhancedInput->BindAction(InventoryEquipmentAction, ETriggerEvent::Started, this, &AMuksiWorldCharacter::OpenInventoryEquipment);
		}
	}
}

//Interaction Start
void AMuksiWorldCharacter::SetCurrentInteractionTarget(AInteractionPointBase* NewTarget)
{
	if (!NewTarget)
	{
		return;
	}

	for (const FInteractionCandidate& Candidate : InteractionCandidates)
	{
		if (Candidate.Point.Get() == NewTarget)
		{
			CurrentInteractionTarget = GetBestInteractionTarget();
			return;
		}
	}

	FInteractionCandidate NewCandidate;
	NewCandidate.Point = NewTarget;
	NewCandidate.EnterOrder = ++InteractionEnterOrderCounter;

	InteractionCandidates.Add(NewCandidate);

	CurrentInteractionTarget = GetBestInteractionTarget();

	UE_LOG(LogTemp, Warning, TEXT("[WorldCharacter] SetInteractionTarget New=%s Best=%s CandidateCount=%d"),
		*GetNameSafe(NewTarget),
		*GetNameSafe(CurrentInteractionTarget),
		InteractionCandidates.Num());
}

void AMuksiWorldCharacter::ClearCurrentInteractionTarget(AInteractionPointBase* TargetToClear)
{
	if (!TargetToClear)
	{
		return;
	}

	const int32 RemovedCount = InteractionCandidates.RemoveAll(
		[TargetToClear](const FInteractionCandidate& Candidate)
		{
			return !Candidate.Point.IsValid() || Candidate.Point.Get() == TargetToClear;
		}
	);

	CurrentInteractionTarget = GetBestInteractionTarget();

	UE_LOG(LogTemp, Warning, TEXT("[WorldCharacter] ClearInteractionTarget Clear=%s Removed=%d Best=%s CandidateCount=%d"),
		*GetNameSafe(TargetToClear),
		RemovedCount,
		*GetNameSafe(CurrentInteractionTarget),
		InteractionCandidates.Num());
}

AInteractionPointBase* AMuksiWorldCharacter::GetCurrentInteractionTarget() const
{
	return GetBestInteractionTarget();
}

AInteractionPointBase* AMuksiWorldCharacter::GetBestInteractionTarget() const
{
	const FInteractionCandidate* BestCandidate = nullptr;

	for (const FInteractionCandidate& Candidate : InteractionCandidates)
	{
		AInteractionPointBase* CandidatePoint = Candidate.Point.Get();
		if (!CandidatePoint)
		{
			continue;
		}

		if (!BestCandidate)
		{
			BestCandidate = &Candidate;
			continue;
		}

		AInteractionPointBase* BestPoint = BestCandidate->Point.Get();
		if (!BestPoint)
		{
			BestCandidate = &Candidate;
			continue;
		}

		const int32 CandidatePriority = CandidatePoint->GetInteractionPriority();
		const int32 BestPriority = BestPoint->GetInteractionPriority();

		if (CandidatePriority > BestPriority)
		{
			BestCandidate = &Candidate;
			continue;
		}

		if (CandidatePriority < BestPriority)
		{
			continue;
		}

		if (Candidate.EnterOrder < BestCandidate->EnterOrder)
		{
			BestCandidate = &Candidate;
			continue;
		}

		if (Candidate.EnterOrder > BestCandidate->EnterOrder)
		{
			continue;
		}

		const float CandidateDistanceSq = FVector::DistSquared(GetActorLocation(), CandidatePoint->GetActorLocation());
		const float BestDistanceSq = FVector::DistSquared(GetActorLocation(), BestPoint->GetActorLocation());

		if (CandidateDistanceSq < BestDistanceSq)
		{
			BestCandidate = &Candidate;
		}
	}

	return BestCandidate ? BestCandidate->Point.Get() : nullptr;
}

void AMuksiWorldCharacter::Interact(const FInputActionValue& Value)
{
	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (UPlayerMode_World* WorldMode = Cast<UPlayerMode_World>(PC->GetCurrentPlayerMode()))
	{
		WorldMode->HandleInteract(this);
	}
}
//Interaction End

void AMuksiWorldCharacter::OpenInventoryEquipment(const FInputActionValue& Value)
{
	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (UPlayerMode_World* WorldMode = Cast<UPlayerMode_World>(PC->GetCurrentPlayerMode()))
	{
		WorldMode->HandleOpenInventoryEquipment(this);
	}
}

UStatComponent* AMuksiWorldCharacter::GetStatComponent() const
{
	return StatComponent;
}

UInventoryComponent* AMuksiWorldCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

UEquipmentComponent* AMuksiWorldCharacter::GetEquipmentComponent() const
{
	return EquipmentComponent;
}

UPlayerCurrencyComponent* AMuksiWorldCharacter::GetPlayerCurrencyComponent() const
{
	return PlayerCurrencyComponent;
}
