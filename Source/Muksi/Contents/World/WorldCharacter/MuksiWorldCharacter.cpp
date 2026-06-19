#include "Muksi/Contents/World/WorldCharacter/MuksiWorldCharacter.h"

#include "Controllers/MuksiPlayerController.h"
#include "Controllers/PlayerMode/PlayerMode_World.h"
#include "Muksi/Contents/World/TownInteractionPoint.h"

#include "Components/StatComponent.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Components/EquipmentComponent.h"
#include "Components/PlayerCurrencyComponent.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"

#include "GameFramework/Controller.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

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

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Component
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	PlayerCurrencyComponent = CreateDefaultSubobject<UPlayerCurrencyComponent>(TEXT("PlayerCurrencyComponent"));
}

void AMuksiWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	// stub
}


void AMuksiWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SetDestinationClickAction)
		{
			// Setup mouse input events
			EnhancedInput->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AMuksiWorldCharacter::OnInputStarted);
			EnhancedInput->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AMuksiWorldCharacter::OnSetDestinationTriggered);
			EnhancedInput->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AMuksiWorldCharacter::OnSetDestinationReleased);
			EnhancedInput->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AMuksiWorldCharacter::OnSetDestinationReleased);
		}

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

void AMuksiWorldCharacter::OnInputStarted()
{
	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (UPlayerMode_World* WorldMode = Cast<UPlayerMode_World>(PC->GetCurrentPlayerMode()))
	{
		WorldMode->HandleWorldInputStarted(this);
	}
}

void AMuksiWorldCharacter::OnSetDestinationTriggered()
{
	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (UPlayerMode_World* WorldMode = Cast<UPlayerMode_World>(PC->GetCurrentPlayerMode()))
	{
		WorldMode->HandleSetDestinationTriggered(this);
	}
}

void AMuksiWorldCharacter::OnSetDestinationReleased()
{
	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (UPlayerMode_World* WorldMode = Cast<UPlayerMode_World>(PC->GetCurrentPlayerMode()))
	{
		WorldMode->HandleSetDestinationReleased(this);
	}
}


//Interaction
void AMuksiWorldCharacter::SetCurrentInteractionTarget(ATownInteractionPoint* NewTarget)
{
	CurrentInteractionTarget = NewTarget;
}

void AMuksiWorldCharacter::ClearCurrentInteractionTarget(ATownInteractionPoint* TargetToClear)
{
	if (CurrentInteractionTarget == TargetToClear)
	{
		CurrentInteractionTarget = nullptr;
	}
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