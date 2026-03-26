// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MuksiPlayerController.h"

#include "InputMappingContext.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "EnhancedInputSubsystems.h"
#include "MuksiSettings/MuksiWorldSettings.h"

AMuksiPlayerController::AMuksiPlayerController()
{
	bShowMouseCursor = true;
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
			CurrentPlayerMode->EnterMode();
			ApplyCurrentPlayerModeIMC();
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Player Mode Type in Map"));
	}
}