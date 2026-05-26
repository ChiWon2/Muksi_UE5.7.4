// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Runtime/CinematicCamera/Public/CineCameraActor.h"


void UPlayerModeBase::EnterMode(AMuksiPlayerController* PlayerController)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Enter Mode"));
	PC = PlayerController;
}

void UPlayerModeBase::ExitMode()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase Exit Mode"));
}

void UPlayerModeBase::HandleLeftClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleLeftClick"));
}

void UPlayerModeBase::HandleRightClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleRightClick"));
}

void UPlayerModeBase::HandlePPressedKey(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandlePPressedKey"));
}


void UPlayerModeBase::HandleTabPressedKey(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleTabPressedKey"));
}

void UPlayerModeBase::HandleIPressedKey(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleIPressedKey"));
}

void UPlayerModeBase::HandleEPressedKey(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleEPressedKey"));
}


void UPlayerModeBase::HandleQPressedKey_Implementation(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerModeBase HandleQPressedKey Blueprint Test"));
}


TObjectPtr<ACineCameraActor> UPlayerModeBase::ApplyStartCamera()
{
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyBattleCamera failed: PC is null"));
		return nullptr;
	}

	UWorld* World = this->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyBattleCamera failed: World is null"));
		return nullptr;
	}

	TArray<AActor*> FoundCameras;

	UGameplayStatics::GetAllActorsWithTag(
		World,
		FName(TEXT("StartCamera")),
		FoundCameras
	);

	if (FoundCameras.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyBattleCamera failed: No actor with tag StartCamera"));
		return nullptr;
	}

	ACineCameraActor* StartCamera = Cast<ACineCameraActor>(FoundCameras[0]);
	if (!StartCamera)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ApplyBattleCamera failed: Found actor is not CineCameraActor. Actor=%s"),
			*GetNameSafe(FoundCameras[0])
		);
		return nullptr;
	}

	

	UE_LOG(LogTemp, Log, TEXT("Camera applied: %s"), *GetNameSafe(StartCamera));
	
	
	return StartCamera;
}


