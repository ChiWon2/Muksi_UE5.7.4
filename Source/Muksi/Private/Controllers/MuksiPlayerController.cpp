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

//Test Zone, Town UI
#include "Muksi/Contents/World/Zone/ZoneManager.h"
#include "Muksi/Contents/World/Zone/ZoneActor.h"
#include "MuksiFunctionLibrary.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/World/Data/TownDataAsset.h"
#include "Widgets/World/Widget_WorldTown.h"
//Test Zone, Town UI


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

//Test Zone, Town UI
void AMuksiPlayerController::SetCurrentZone(AZoneActor* NewZone)
{
	if (ZoneManager)
	{
		ZoneManager->SetCurrentZone(NewZone);
	}
}

AZoneActor* AMuksiPlayerController::GetCurrentZone() const
{
	return ZoneManager ? ZoneManager->GetCurrentZone() : nullptr;
}

FZoneData AMuksiPlayerController::GetCurrentZoneData() const
{
	return ZoneManager ? ZoneManager->GetCurrentZoneData() : FZoneData();
}

void AMuksiPlayerController::OpenTownUI(UTownDataAsset* InTownData)
{
	UE_LOG(LogTemp, Log, TEXT("OpenTownUI entered"));

	if (CurrentTownWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("OpenTownUI skipped: already open"));
		return;
	}

	const FZoneData ZoneData = GetCurrentZoneData();
	if (!ZoneData.bCanOpenTownUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI blocked: current zone does not allow Town UI"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: UISubsystem is null"));
		return;
	}

	TSoftClassPtr<UWidget_ActivatableBase> ResolvedTownWidgetClass = TownWidgetClass;
	if (ResolvedTownWidgetClass.IsNull())
	{
		ResolvedTownWidgetClass = UMuksiFunctionLibrary::GetMuksiSoftWidgetClassByTag(
			MuksiGameplayTag::Muksi_Widget_World_Town
		);
	}

	if (ResolvedTownWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTownUI failed: TownWidgetClass is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		this,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ResolvedTownWidgetClass,
		true,
		[this, InTownData](UWidget_ActivatableBase* CreatedWidget)
		{
			if (!CreatedWidget)
			{
				return;
			}

			CurrentTownWidget = CreatedWidget;

			if (UWidget_WorldTown* TownWidget = Cast<UWidget_WorldTown>(CreatedWidget))
			{
				TownWidget->InitializeTown(InTownData);
				UE_LOG(LogTemp, Warning, TEXT("Town UI initialized with DataAsset: %s"), *GetNameSafe(InTownData));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CreatedWidget is not UWidget_WorldTown: %s"), *GetNameSafe(CreatedWidget));
			}

			ApplyTownUIInputMode(CreatedWidget);

			UE_LOG(LogTemp, Log, TEXT("Town UI created: %s"), *GetNameSafe(CreatedWidget));
		},
		[this](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("Town UI pushed: %s"), *GetNameSafe(PushedWidget));
		}
	);
}

void AMuksiPlayerController::CloseTownUI()
{
	UE_LOG(LogTemp, Warning, TEXT("CloseTownUI entered"));

	if (!CurrentTownWidget)
	{
		RestoreWorldInputMode();
		UE_LOG(LogTemp, Warning, TEXT("CloseTownUI: null path done"));
		return;
	}

	UWidget_ActivatableBase* TownWidget = CurrentTownWidget;
	CurrentTownWidget = nullptr;

	RestoreWorldInputMode();
	UE_LOG(LogTemp, Warning, TEXT("CloseTownUI: after RestoreWorldInputMode"));

	if (TownWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CloseTownUI: before DeactivateWidget %s"), *GetNameSafe(TownWidget));
		TownWidget->DeactivateWidget();
	}

	UE_LOG(LogTemp, Warning, TEXT("Town UI closed"));
}



void AMuksiPlayerController::ApplyTownUIInputMode(UWidget_ActivatableBase* FocusWidget)
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}

	SetInputMode(InputMode);

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	UE_LOG(LogTemp, Log, TEXT("ApplyTownUIInputMode: Town UI focus applied"));
}


void AMuksiPlayerController::RestoreWorldInputMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	UE_LOG(LogTemp, Log, TEXT("RestoreWorldInputMode: GameAndUI applied, cursor visible, click events enabled"));
}


//Test Zone, Town UI

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
