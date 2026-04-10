// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MuksiPlayerController.h"

#include "InputMappingContext.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "EnhancedInputSubsystems.h"
#include "MuksiSettings/MuksiWorldSettings.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

#include "NativeGameplayTags.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"


#include "MuksiDebugHelper.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "Widgets/Battle/Widget_BattleMainScreen.h"


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
	
	PlayerModeMap.Empty();
	for (const TPair<EPlayerModeType, TSubclassOf<UPlayerModeBase>>& Pair : PlayerModeClasses)
	{
		if (!Pair.Value){continue;}
		
		UPlayerModeBase* NewPlayerMode = NewObject<UPlayerModeBase>(this, Pair.Value);
		if (!NewPlayerMode){continue;}
		
		PlayerModeMap.Add(Pair.Key, NewPlayerMode);
	}
	
	ChangePlayerMode(StartModeType);
	
}

void AMuksiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LeftClickAction)
		{
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AMuksiPlayerController::OnLeftClick);
			UE_LOG(LogTemp, Log, TEXT("Bind LeftClickAction"));
		}
	}
}

void AMuksiPlayerController::OnLeftClick(const FInputActionValue& Value)
{
	FHitResult HitResult;

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
	}
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

void AMuksiPlayerController::ChangePlayerMode(EPlayerModeType ModeType)
{
	if (TObjectPtr<UPlayerModeBase>* FoundMode = PlayerModeMap.Find(ModeType))
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
	}
}