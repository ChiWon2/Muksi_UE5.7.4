// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "PlayBattleCameraExecution.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UPlayBattleCameraExecution : public UBattleExecution
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;
};
