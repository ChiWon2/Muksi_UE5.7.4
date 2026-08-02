// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Widgets/Battle/CAW/Widget_CharacterData.h"
#include "Subsystems/MuksiUISubsystem.h"

#include "MuksiDebugHelper.h"
#include "MuksiGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/SelectGridInterface.h"
#include "Muksi/Contents/Battle/Interfaces/SelectableCharacterInterface.h"
#include "Widgets/Battle/Widget_BattleMainScreen.h"

namespace
{
	bool ResolveGridCoordFromCursorHit(
		ABattleManager* BattleManager,
		const FHitResult& HitResult,
		FHexOffsetCoord& OutCoord,
		ABattleGridTile*& OutTile)
	{
		OutCoord = FHexOffsetCoord::Invalid();
		OutTile = nullptr;

		if (!BattleManager)
		{
			return false;
		}

		ABattleGridManager* GridManager = BattleManager->GetBattleGridManager();
		if (!GridManager)
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
			OutTile = GridManager->GetTileActorByCoord(OutCoord);
			return OutCoord.IsValid() && OutTile;
		}

		// 무기, ChildActor, 별도 Collision Component가 캐릭터/타일보다 먼저
		// Visibility Trace를 맞는 경우에도 ImpactPoint에 가장 가까운 Grid Tile을
		// 선택한다. Selection 종류와 점유 여부는 이 좌표 획득 단계에서 검사하지 않는다.
		float BestDistanceSquared = TNumericLimits<float>::Max();
		for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
		{
			for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
			{
				const FHexOffsetCoord Coord(X, Y);
				ABattleGridTile* Tile = GridManager->GetTileActorByCoord(Coord);
				if (!Tile)
				{
					continue;
				}

				const FVector Delta = GridManager->GetWorldLocationByCoord(Coord) - HitResult.ImpactPoint;
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

	BattleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass( this, ABattleManager::StaticClass()));

	if (!BattleManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerMode_Battle]EnterMode - BattleManager not found"));
		return;
	}

	BattleMainScreen = BattleManager->GetBattleMainScreen();
	if (!BattleMainScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerMode_Battle]EnterMode - BattleMainScreen not found"));
	}
}

void UPlayerMode_Battle::ExitMode()
{
	if (BattleManager)
	{
		BattleManager->CancelPlayerCardTargeting();
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

	if (!BattleManager)
	{
		return;
	}

	if (BattleManager->IsPlayerCardTargeting())
	{
		BattleManager->ConfirmPlayerCardTargetingStep();
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

	if (!BattleManager)
	{
		return;
	}

	// Do not gate undo through IsPlayerCardTargeting(). The session itself owns
	// whether one confirmed step can be restored, including Completed state.
	if (BattleManager->UndoPlayerCardTargetingStep())
	{
		return;
	}

	// No confirmed step remains: a right click cancels the whole card targeting.
	if (BattleManager->HasActivePlayerTargetingSession())
	{
		BattleManager->CancelPlayerCardTargeting();
	}
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
		ResolveGridCoordFromCursorHit(BattleManager, HitResult, HoveredCoord, NewHoveredGridTile);
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
	if (!BattleManager || !BattleManager->IsPlayerCardTargeting())
	{
		return;
	}

	if (!bHasHitResult)
	{
		BattleManager->UpdatePlayerTargetingAim(FVector::ZeroVector, false);
		BattleManager->UpdatePlayerTargetingCandidate(FHexOffsetCoord::Invalid());
		return;
	}

	BattleManager->UpdatePlayerTargetingAim(HitResult.ImpactPoint, true);

	FHexOffsetCoord CandidateCoord = FHexOffsetCoord::Invalid();
	ABattleGridTile* CandidateTile = nullptr;
	ResolveGridCoordFromCursorHit(BattleManager, HitResult, CandidateCoord, CandidateTile);

	// 좌표 획득은 Point/Direction/Target/AreaCenter/Destination 모두 공통이다.
	// 점유/경로 제한은 TargetingStepCardData의 Purpose 검증 단계에서만 적용한다.
	BattleManager->UpdatePlayerTargetingCandidate(CandidateCoord);
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
