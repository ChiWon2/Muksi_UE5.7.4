// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"
#include "Subsystems/MuksiUISubsystem.h"

#include "MuksiDebugHelper.h"
#include "Engine/AssetManager.h"
#include "MuksiGameplayTags.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/SelectGridInterface.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"



void UPlayerMode_Battle::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	
	// CharacterData UI 미리 로드
	PreloadCharacterDataWidget();

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		if (ABattleManager* BattleManager = ManagerSubsystem->GetManager<ABattleManager>())
		{
			BattleTargetingManager = BattleManager->GetBattleTargetingManager();
			BattleGridManager = BattleManager->GetBattleGridManager();
		}
	}

	if (!BattleTargetingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerMode_Battle] EnterMode - BattleTargetingManager not found"));
		return;
	}
}

void UPlayerMode_Battle::ExitMode()
{
	if (BattleTargetingManager)
	{
        BattleTargetingManager->CancelPlayerTargeting();
	}

	BattleTargetingManager = nullptr;
	BattleGridManager = nullptr;

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

	FHitResult HitResult;
	const bool bHasHitResult = PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	UpdateHoveredGridTile(HitResult, bHasHitResult);
	UpdateCardTargeting(HitResult, bHasHitResult);
}

void UPlayerMode_Battle::HandleLeftClick(const FInputActionValue& Value)
{
	Super::HandleLeftClick(Value);


	SelectedActor = nullptr;

	if (!PC)
	{
		return;
	}

	if (!BattleTargetingManager)
	{
		return;
	}

	FHitResult HitResult;
	const bool bHasHitResult = PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);


	if (BattleTargetingManager->RequestUpdatePlayerTargeting(HitResult, bHasHitResult)
		&& BattleTargetingManager->RequestConfirmPlayerTargeting())
	{
		return;
	}

	if (!bHasHitResult)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	if (!HitActor)
	{
		return;
	}

	if (HitActor->GetClass()->ImplementsInterface(USelectableCharacterInterface::StaticClass()))
	{
		SelectedActor = HitActor;

		if (ABattleCharacterBase* SelectedCharacter = Cast<ABattleCharacterBase>(HitActor))
		{
			FocusCameraOnCharacter(SelectedCharacter);
		}

		PushCharacterDataWidget();
		return;
	}

	if (HitActor->GetClass()->ImplementsInterface(USelectGridInterface::StaticClass()))
	{
		ISelectGridInterface::Execute_OnGridSelected(HitActor);
	}
}

void UPlayerMode_Battle::HandleRightClick(const FInputActionValue& Value)
{
	Super::HandleRightClick(Value);

	if (!BattleTargetingManager)
	{
		return;
	}

	BattleTargetingManager->RequestUndoOrCancelPlayerTargeting();
}

void UPlayerMode_Battle::HandleRPressedKey(const FInputActionValue& Value)
{
	Super::HandleRPressedKey(Value);
}

void UPlayerMode_Battle::UpdateHoveredGridTile(const FHitResult& HitResult, bool bHasHitResult)
{
	ABattleGridTile* NewHoveredGridTile = nullptr;
	FHexOffsetCoord HoveredCoord = FHexOffsetCoord::Invalid();

	if (bHasHitResult && BattleGridManager && BattleGridManager->GetPresentationCoordFromHit(HitResult, HoveredCoord))
	{
		NewHoveredGridTile = BattleGridManager->GetTileActorByCoord(HoveredCoord);
	}

	if (HoveredGridTile == NewHoveredGridTile)
	{
		return;
	}

	if (HoveredGridTile)
	{
		HoveredGridTile->OnHoverEnd();
	}

	HoveredGridTile = NewHoveredGridTile;

	if (HoveredGridTile)
	{
		HoveredGridTile->OnHoverBegin();
	}
}

void UPlayerMode_Battle::UpdateCardTargeting(const FHitResult& HitResult, bool bHasHitResult)
{
	if (BattleTargetingManager)
	{
		BattleTargetingManager->RequestUpdatePlayerTargeting(HitResult, bHasHitResult);
	}
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

void UPlayerMode_Battle::FocusCameraOnCharacter(ABattleCharacterBase* Character)
{
	if (!IsValid(Character))
	{
		return;
	}

	UMuksiWorldManagerSubsystem* ManagerSubsystem =
		UMuksiWorldManagerSubsystem::Get(this);

	if (!IsValid(ManagerSubsystem))
	{
		return;
	}

	ABattleCameraManager* CameraManager =
		ManagerSubsystem->GetManager<ABattleCameraManager>();

	if (!IsValid(CameraManager))
	{
		return;
	}

	CameraManager->FocusCharacter(Character);
}

void UPlayerMode_Battle::PreloadCharacterDataWidget()
{
	if (WidgetCharacterDataClass.IsNull())
	{
		return;
	}

	// 이미 로드돼 있다면 그대로 보관
	if (WidgetCharacterDataClass.IsValid())
	{
		LoadedCharacterDataWidgetClass = WidgetCharacterDataClass.Get();

		return;
	}

	CharacterDataWidgetLoadHandle = UAssetManager::Get()
		.GetStreamableManager()
		.RequestAsyncLoad(
			WidgetCharacterDataClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(
				this,
				&UPlayerMode_Battle::
					HandleCharacterDataWidgetPreloaded
			)
		);
}

void UPlayerMode_Battle::HandleCharacterDataWidgetPreloaded()
{
	LoadedCharacterDataWidgetClass = WidgetCharacterDataClass.Get();

	CharacterDataWidgetLoadHandle.Reset();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("CharacterData Widget Preloaded")
	);
}
