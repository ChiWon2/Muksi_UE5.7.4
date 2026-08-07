// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"
#include "Subsystems/MuksiUISubsystem.h"

#include "MuksiDebugHelper.h"
#include "MuksiGameplayTags.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/SelectGridInterface.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"

namespace
{
	bool ResolveGridCoordFromCursorHit(
		ABattleGridManager* BattleGridManager,
		const FHitResult& HitResult,
		FHexOffsetCoord& OutCoord,
		ABattleGridTile*& OutTile)
	{
		OutCoord = FHexOffsetCoord::Invalid();
		OutTile = nullptr;

		if (!BattleGridManager)
		{
			return false;
		}

		AActor* HitActor = HitResult.GetActor();

		if (ABattleGridTile* HitTile = Cast<ABattleGridTile>(HitActor))
		{
			OutCoord = HitTile->GetGridCoord();
			OutTile = HitTile;
			return true;
		}

		if (const ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitActor))
		{
			OutCoord = HitCharacter->GetCharacterCoord();
			OutTile = BattleGridManager->GetTileActorByCoord(OutCoord);
			return OutCoord.IsValid() && OutTile;
		}

		// 무기, ChildActor, 별도 Collision Component가 캐릭터/타일보다 먼저
		// Visibility Trace를 맞는 경우에도 ImpactPoint에 가장 가까운 Grid Tile을
		// 선택한다. Selection 종류와 점유 여부는 이 좌표 획득 단계에서 검사하지 않는다.
		float BestDistanceSquared = TNumericLimits<float>::Max();
		for (int32 X = 0; X < BattleGridManager->GetGridWidth(); ++X)
		{
			for (int32 Y = 0; Y < BattleGridManager->GetGridHeight(); ++Y)
			{
				const FHexOffsetCoord Coord(X, Y);
				ABattleGridTile* Tile = BattleGridManager->GetTileActorByCoord(Coord);
				if (!Tile)
				{
					continue;
				}

				const FVector Delta = BattleGridManager->GetWorldLocationByCoord(Coord) - HitResult.ImpactPoint;
				const float DistanceSquared = FVector2D(Delta.X, Delta.Y).SizeSquared();
				if (DistanceSquared < BestDistanceSquared)
				{
					BestDistanceSquared = DistanceSquared;
					OutCoord = Coord;
					OutTile = Tile;
				}
			}
		}

		return OutCoord.IsValid() && OutTile;
	}
}

void UPlayerMode_Battle::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		BattleTargetingManager = ManagerSubsystem->GetManager<ABattleTargetingManager>();
		BattleGridManager = ManagerSubsystem->GetManager<ABattleGridManager>();
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
		BattleTargetingManager->RequestCancelPlayerTargeting();
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

	if (BattleTargetingManager->RequestConfirmPlayerTargeting())
	{
		return;
	}

	FHitResult HitResult;

	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
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

	if (bHasHitResult)
	{
		ResolveGridCoordFromCursorHit(BattleGridManager, HitResult, HoveredCoord, NewHoveredGridTile);
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
