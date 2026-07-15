// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"
#include "Subsystems/MuksiUISubsystem.h"

#include "MuksiDebugHelper.h"
#include "MuksiGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/SelectGridInterface.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingContext.h"
#include "Widgets/Battle/Widget_BattleMainScreen.h"

void UPlayerMode_Battle::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;

	/*FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);*/

	UE_LOG(LogTemp, Warning, TEXT("PlayerMode_Battle"));

	BattleManager = Cast<ABattleManager>(
		UGameplayStatics::GetActorOfClass(
			this,
			ABattleManager::StaticClass()
		)
	);

	if (!BattleManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerMode_Battle::EnterMode - BattleManager not found"));
		return;
	}

	InitializeBattleTestData();
}

void UPlayerMode_Battle::ExitMode()
{
	if (BattleManager)
	{
		BattleManager->CancelCurrentCardTargeting();
	}

	Super::ExitMode();

	PC->SetIgnoreLookInput(false);
	PC->SetIgnoreMoveInput(false);

	PC->bShowMouseCursor = false;
	PC->bEnableClickEvents = false;
	PC->bEnableMouseOverEvents = false;
}

void UPlayerMode_Battle::TickPlayerMode()
{
	Super::TickPlayerMode();

	UpdateHoveredGridTile();
}

//***************** Test

void UPlayerMode_Battle::HandleLeftClick(const FInputActionValue& Value)
{
	Super::HandleLeftClick(Value);

	SelectedActor = nullptr;

	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerMode_Battle::HandleLeftClick - PC is nullptr"));
		return;
	}

	if (!BattleManager)
	{
		return;
	}

	if (BattleManager->IsCardTargeting())
	{
		if (!BattleManager->ConfirmCurrentCardTargeting())
		{
			return;
		}

		if (!BattleMainScreen)
		{
			BattleMainScreen = BattleManager->GetBattleMainScreen();
		}

		if (!BattleMainScreen)
		{
			return;
		}

		BattleMainScreen->PlayerSelectCardFinish = true;
		BattleManager->ExchangeN_EndReady();
		return;
	}

	FHitResult HitResult;

	const bool bHit = PC->GetHitResultUnderCursorByChannel(
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
		SelectedActor = HitActor;
		PushCharacterDataWidget();
	}
	else if (HitActor->GetClass()->ImplementsInterface(USelectGridInterface::StaticClass()))
	{
		ISelectGridInterface::Execute_OnGridSelected(HitActor);
	}
	else
	{
		Debug::Print(FString::Printf(TEXT("Not Selectable : %s"), *HitActor->GetName()));
	}
}

void UPlayerMode_Battle::HandleRPressedKey(const FInputActionValue& Value)
{
	Super::HandleRPressedKey(Value);
}

void UPlayerMode_Battle::UpdateHoveredGridTile()
{
	if (!PC)
	{
		return;
	}

	if (!BattleManager)
	{
		return;
	}

	FHitResult HitResult;

	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		HoveredGridTile = nullptr;
		return;
	}

	HoveredGridTile = Cast<ABattleGridTile>(HitResult.GetActor());

	if (BattleManager->IsCardTargeting())
	{
		UpdateCardTargeting(HitResult);
	}
}

void UPlayerMode_Battle::UpdateCardTargeting(const FHitResult& HitResult)
{
	if (!BattleManager)
	{
		return;
	}

	ABattleGridManager* GridManager = BattleManager->GetBattleGridManager();

	if (!GridManager)
	{
		return;
	}

	FMuksiCardTargetingContext Context;
	Context.AimWorldLocation = HitResult.ImpactPoint;

	AActor* HitActor = HitResult.GetActor();

	if (ABattleGridTile* HitTile = Cast<ABattleGridTile>(HitActor))
	{
		Context.HoveredCoord = HitTile->GetGridCoord();

		const FBattleGridCell* Cell = GridManager->GetCell(Context.HoveredCoord);

		if (Cell)
		{
			if (ABattleCharacterBase* CandidateCharacter = Cast<ABattleCharacterBase>(Cell->OccupyingActor))
			{
				Context.CandidateCharacters.AddUnique(CandidateCharacter);
			}
		}
	}
	else if (ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitActor))
	{
		Context.HoveredCoord = HitCharacter->GetCharacterPosition();
		Context.CandidateCharacters.AddUnique(HitCharacter);
	}

	BattleManager->UpdateCurrentCardTargeting(Context);
}

void UPlayerMode_Battle::InitializeBattleTestData()
{
	/*if (!BattleCardDataTable || !TestPlayerCharacterDataAsset || !TestEnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerMode::InitializeBattleTestData - Required asset is missing"));
		return;
	}

	PlayerCharacterData = NewObject<UCharacterDataBase>(this);
	EnemyCharacterData = NewObject<UCharacterDataBase>(this);

	if (PlayerCharacterData)
	{
		PlayerCharacterData->InitializeFromDataAsset(TestPlayerCharacterDataAsset);
	}

	if (EnemyCharacterData)
	{
		EnemyCharacterData->InitializeFromDataAsset(TestEnemyCharacterDataAsset);
	}*/
}

void UPlayerMode_Battle::PushCharacterDataWidget()
{
	UE_LOG(LogTemp, Log, TEXT("pushCharacterDataWidget Test"));

	if (WidgetCharacterDataClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Push Character Data Widget failed"));
		return;
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(PC);

	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UISubsystem is NULL"));
		return;
	}

	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PC is NULL"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		WidgetCharacterDataClass,
		true,
		[this](UWidget_ActivatableBase* CreateWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("Before Push : %s"), *GetNameSafe(CreateWidget));

			if (UWidget_CharacterData* CharacterDataWidget = Cast<UWidget_CharacterData>(CreateWidget))
			{
				//초기화
				if (SelectedActor)
				{
					//초기화
					if (SelectedActor)
					{
						//CharacterDataWidget->GetClickedActor(SelectedActor);
						CharacterDataWidget->GetCharacterData(Cast<ABattleCharacterBase>(SelectedActor));
					}
				}
			}
		},
		[this](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("After Push: %s"), *GetNameSafe(PushedWidget));
		}
	);
}