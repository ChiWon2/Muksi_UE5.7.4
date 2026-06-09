// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_Test.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UPlayerMode_Test : public UPlayerModeBase
{
	GENERATED_BODY()
	
	//~ Begin UPlayerModeBase Interface
	virtual void EnterMode(AMuksiPlayerController* PlayerController)override;
	virtual void ExitMode()override;
	virtual int32 GetInputMappingPriority() const override { return 10; }

	//~ End UPlayerModeBase Interface
};
