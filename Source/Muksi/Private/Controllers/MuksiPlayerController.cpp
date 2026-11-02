// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/MuksiPlayerController.h"

#include "InputMappingContext.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "EnhancedInputSubsystems.h"
#include "MuksiSettings/MuksiWorldSettings.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

AMuksiPlayerController::AMuksiPlayerController()
{
	bShowMouseCursor = true;

}

void AMuksiPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentPlayerMode)
	{
		CurrentPlayerMode->TickPlayerMode();
	}
}

void AMuksiPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TSubclassOf<UPlayerModeBase> StartPlayerModeClass = nullptr;

	if (AMuksiWorldSettings* MuksiWorldSettings = Cast<AMuksiWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		StartPlayerModeClass = MuksiWorldSettings->StartPlayerModeClass;
	}

	if (!StartPlayerModeClass)
	{
		StartPlayerModeClass = UPlayerModeBase::StaticClass();
	}

	ChangePlayerMode(StartPlayerModeClass);

}

void AMuksiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UE_LOG(LogTemp, Verbose, TEXT("SetupInputComponent"));
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LeftClickAction)
		{
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnLeftClick);
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &AMuksiPlayerController::OnLeftClickReleased);
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Canceled, this, &AMuksiPlayerController::OnLeftClickReleased);
			UE_LOG(LogTemp, Log, TEXT("Bind LeftClickAction"));
		}
		if (RightClickAction)
		{
			EnhancedInput->BindAction(RightClickAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnRightClick);
			EnhancedInput->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &AMuksiPlayerController::OnRightClickTriggered);
			UE_LOG(LogTemp, Log, TEXT("Bind RightClickAction"));
		}
		if (PPressAction)
		{
			EnhancedInput->BindAction(PPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnPKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind PPressAction"));
		}
		if (QPressAction)
		{
			EnhancedInput->BindAction(QPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnQKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind QPressAction"));
		}
		if (EPressAction)
		{
			EnhancedInput->BindAction(EPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnEKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind EPressAction"));
		}
		if (IPressAction)
		{
			EnhancedInput->BindAction(IPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnIKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind IPressAction"));
		}
		if (TabPressAction)
		{
			EnhancedInput->BindAction(TabPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnTabKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind TabPressAction"));
		}

		if (RPressAction)
		{
			EnhancedInput->BindAction(RPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnRKeyPressed);
			UE_LOG(LogTemp, Log, TEXT("Bind RPressAction"));
		}

		if (NumberKeyAction)
			EnhancedInput->BindAction(NumberKeyAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnNumberKeyPressed);

		if (MouseWheelAction)
			EnhancedInput->BindAction(MouseWheelAction, ETriggerEvent::Triggered, this, &AMuksiPlayerController::OnMouseWheel);

		if (CPressAction)
			EnhancedInput->BindAction(CPressAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnCKeyPressed);
	}
}

void AMuksiPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(
	   LogTemp,
	   Warning,
	   TEXT(
		   "[WorldReturn][OnPossess] Controller=%s Pawn=%s Class=%s Location=%s"
	   ),
	   *GetNameSafe(this),
	   *GetNameSafe(InPawn),
	   InPawn ? *GetNameSafe(InPawn->GetClass()) : TEXT("None"),
	   InPawn
		   ? *InPawn->GetActorLocation().ToString()
		   : TEXT("None")
   );
}

void AMuksiPlayerController::OnLeftClick(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleLeftClick(Value);
}

void AMuksiPlayerController::OnLeftClickReleased(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleLeftClickReleased(Value);
}

void AMuksiPlayerController::OnCKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleCPressedKey(Value);
}

void AMuksiPlayerController::OnRightClick(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleRightClick(Value);
}

void AMuksiPlayerController::OnRightClickTriggered(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleRightClickTriggered(Value);
}

void AMuksiPlayerController::OnNumberKeyPressed(const FInputActionValue& Value)
{
	if (!CurrentPlayerMode)
		return;

	const int32 NumberKey = FMath::RoundToInt(Value.Get<float>());

	if (NumberKey < 1 || NumberKey > 9)
		return;

	CurrentPlayerMode->HandleNumberKeyPressed(NumberKey);
}

void AMuksiPlayerController::OnEKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleEPressedKey(Value);
}

void AMuksiPlayerController::OnMouseWheel(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleMouseWheel(Value);
}

void AMuksiPlayerController::OnQKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleQPressedKey(Value);
}

void AMuksiPlayerController::OnIKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleIPressedKey(Value);
}

void AMuksiPlayerController::OnTabKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleTabPressedKey(Value);
}

void AMuksiPlayerController::OnRKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandleRPressedKey(Value);
}

//Test Bind Input
void AMuksiPlayerController::OnPKeyPressed(const FInputActionValue& Value)
{
	//Blue print에서 Bind 해서 작동시키는 모드
	OnPPressedInput.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("OnPKeyPressed"));
	//PlayerMode의 P 입력 모드
	if (CurrentPlayerMode)
		CurrentPlayerMode->HandlePPressedKey(Value);
}

FGameplayTag AMuksiPlayerController::GetStartupWidgetTag() const
{
	if (!GetWorld())
		return FGameplayTag();

	if (AMuksiWorldSettings* MuksiWorldSettings = Cast<AMuksiWorldSettings>(GetWorld()->GetWorldSettings()))
	{
		return MuksiWorldSettings->GetStartupWidgetTag();
	}
	return FGameplayTag();
}

void AMuksiPlayerController::ApplyCurrentPlayerModeIMC()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->ClearAllMappings();

			if (CurrentPlayerMode)
			{
				if (UInputMappingContext* IMC = CurrentPlayerMode->GetInputMappingContext())
				{
					InputSubsystem->AddMappingContext(IMC, 0);
					UE_LOG(LogTemp, Log, TEXT("Input Subsystem Add Mapping Context"));
				}
			}
		}
	}
}

UPlayerModeBase* AMuksiPlayerController::GetCurrentPlayerMode() const
{
	return CurrentPlayerMode;
}

void AMuksiPlayerController::ChangePlayerMode(TSubclassOf<UPlayerModeBase> NewPlayerModeClass)
{

	if (!NewPlayerModeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ChangePlayerMode failed: NewPlayerModeClass is null"));
		return;
	}

	if (CurrentPlayerMode && CurrentPlayerMode->GetClass() == NewPlayerModeClass)
	{
		UE_LOG(LogTemp, Log, TEXT("ChangePlayerMode skipped: already current mode"));
		return;
	}

	if (CurrentPlayerMode)
	{
		CurrentPlayerMode->ExitMode();
		CurrentPlayerMode = nullptr;
	}

	UPlayerModeBase* NewPlayerMode = NewObject<UPlayerModeBase>(this, NewPlayerModeClass);
	if (!NewPlayerMode)
	{
		UE_LOG(LogTemp, Error, TEXT("ChangePlayerMode failed: NewObject failed"));
		return;
	}

	CurrentPlayerMode = NewPlayerMode;
	CurrentPlayerMode->EnterMode(this);

	ApplyCurrentPlayerModeIMC();

	UE_LOG(LogTemp, Log, TEXT("Changed PlayerMode: %s"), *GetNameSafe(CurrentPlayerMode));
}
