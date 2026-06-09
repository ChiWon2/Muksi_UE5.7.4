// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MuksiPlayerController.h"

#include "InputMappingContext.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "EnhancedInputSubsystems.h"
#include "MuksiSettings/MuksiWorldSettings.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "CineCameraActor.h"

#include "NativeGameplayTags.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"


//Test Data Subsystem
#include "Subsystems/MuksiPlayerDataSubsystem.h"

#include "MuksiDebugHelper.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/MuksiUISubsystem.h"


AMuksiPlayerController::AMuksiPlayerController()
{
	bShowMouseCursor = true;

}


void AMuksiPlayerController::PushSoftWidget()
{
	if (UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this))
	{
		UISubsystem->PushSoftWidgetToStackAsync(
			this, MuksiGameplayTag::Muksi_WidgetStack_GameHud,
			WidgetCharacterDataClass,
			true,
			[this](UWidget_ActivatableBase* CreateWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("Before Push : %s"), *GetNameSafe(CreateWidget));
				if (UWidget_CharacterData* CharacterData = Cast<UWidget_CharacterData>(CreateWidget))
				{
					//초기화
				}
			},
			[this](UWidget_ActivatableBase* PushedWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("After Push: %s"), *GetNameSafe(PushedWidget));
			}
		);
	}
}

void AMuksiPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	/*PlayerModeMap.Empty();
	for (const TPair<EPlayerModeType, TSubclassOf<UPlayerModeBase>>& Pair : PlayerModeClasses)
	{
		if (!Pair.Value){continue;}
		
		UPlayerModeBase* NewPlayerMode = NewObject<UPlayerModeBase>(this, Pair.Value);
		if (!NewPlayerMode){continue;}
		
		PlayerModeMap.Add(Pair.Key, NewPlayerMode);
	}*/
	
	
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
	UE_LOG(LogTemp, Warning, TEXT("SetupInputComponent TEST"));
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LeftClickAction)
		{
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnLeftClick);
			UE_LOG(LogTemp, Log, TEXT("Bind LeftClickAction"));
		}
		if (RightClickAction)
		{
			EnhancedInput->BindAction(RightClickAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnLeftClick);
			UE_LOG(LogTemp, Log, TEXT("Bind LeftClickAction"));
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
	}
}

void AMuksiPlayerController::OnLeftClick(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleLeftClick(Value);}
	/*FHitResult HitResult;

	const bool bHit = GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		HitResult
	);

	if (!bHit)
	{
		Debug::Print(TEXT("Nothing Hit"));
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		Debug::Print(TEXT("Hit, But No Actor"));
		return;
	}

	if (HitActor->GetClass()->ImplementsInterface(USelectableCharacterInterface::StaticClass()))
	{
		ClickedActor = HitActor;
		PushSoftWidget();
	}
	else
	{
		Debug::Print(FString::Printf(TEXT("Not Selectable : %s"), *HitActor->GetName()));
	}*/
}

void AMuksiPlayerController::OnRightClick(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleRightClick(Value);}
}



void AMuksiPlayerController::OnEKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleEPressedKey(Value);}
}

void AMuksiPlayerController::OnQKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleQPressedKey(Value);}
}

void AMuksiPlayerController::OnIKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleIPressedKey(Value);}
}

void AMuksiPlayerController::OnTabKeyPressed(const FInputActionValue& Value)
{
	if (CurrentPlayerMode){CurrentPlayerMode->HandleTabPressedKey(Value);}
}


//Test Bind Input
void AMuksiPlayerController::OnPKeyPressed(const FInputActionValue& Value)
{
	//Blue print에서 Bind 해서 작동시키는 모드
	OnPPressedInput.Broadcast();
	
	UE_LOG(LogTemp, Log, TEXT("OnPKeyPressed"));
	//PlayerMode의 P 입력 모드
	if (CurrentPlayerMode){CurrentPlayerMode->HandlePPressedKey(Value);}
}

void AMuksiPlayerController::ApplyInputMappingFromMode(UPlayerModeBase* InMode)
{
	if (!InMode)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	if (CurrentMappingContext)
	{
		Subsystem->RemoveMappingContext(CurrentMappingContext);
		CurrentMappingContext = nullptr;
	}

	if (UInputMappingContext* NewContext = InMode->GetInputMappingContext())
	{
		Subsystem->AddMappingContext(NewContext, InMode->GetInputMappingPriority());
		CurrentMappingContext = NewContext;
	}
}

FGameplayTag AMuksiPlayerController::GetStartupWidgetTag() const
{
	if (!GetWorld()){return FGameplayTag();}
	
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
	/*if (TObjectPtr<UPlayerModeBase>* FoundMode = PlayerModeMap.Find(ModeType))
	{
		CurrentModeType = ModeType;
		if (CurrentPlayerMode == *FoundMode){return;}
		if (CurrentPlayerMode)
		{
			CurrentPlayerMode->ExitMode();
		}
		CurrentPlayerMode = *FoundMode;
		
		if (CurrentPlayerMode)
		{
			CurrentPlayerMode->EnterMode(this);
			ApplyCurrentPlayerModeIMC();
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Player Mode Type in Map"));
	}*/
	
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
	
	/*ACineCameraActor* ViewCamera = CurrentPlayerMode->ApplyStartCamera();
	SetViewTargetWithBlend(ViewCamera, 0.0f);*/

	ApplyCurrentPlayerModeIMC();

	UE_LOG(LogTemp, Log, TEXT("Changed PlayerMode: %s"), *GetNameSafe(CurrentPlayerMode));
}
