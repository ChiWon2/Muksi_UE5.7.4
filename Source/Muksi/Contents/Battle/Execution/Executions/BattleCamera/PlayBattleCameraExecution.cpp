// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Execution/Executions/BattleCamera/PlayBattleCameraExecution.h"

#include "PlayBattleCameraExecutionData.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

void UPlayBattleCameraExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FPlayBattleCameraExecutionData* CameraData =
		Context.GetExecutionData<FPlayBattleCameraExecutionData>();

	if (!CameraData)
	{
		FinishExecution(OnFinished);
		return;
	}

	if (!CameraData->LevelSequence)
	{
		FinishExecution(OnFinished);
		return;
	}

	if (!IsValid(Context.Attacker))
	{
		FinishExecution(OnFinished);
		return;
	}

	UMuksiWorldManagerSubsystem* ManagerSubsystem =
		UMuksiWorldManagerSubsystem::Get(Context.Attacker);

	if (!IsValid(ManagerSubsystem))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayBattleCameraExecution: ManagerSubsystem is invalid")
		);

		FinishExecution(OnFinished);
		return;
	}

	ABattleCameraManager* CameraManager =
		ManagerSubsystem->GetManager<ABattleCameraManager>();

	if (!IsValid(CameraManager))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayBattleCameraExecution: BattleCameraManager is invalid")
		);

		FinishExecution(OnFinished);
		return;
	}
	
	CameraManager->PlayAttackCameraSequence(CameraData->LevelSequence,Context.Attacker);

	FinishExecution(OnFinished);
}

const UScriptStruct* UPlayBattleCameraExecution::GetExecutionDataStruct() const
{
	return FPlayBattleCameraExecutionData::StaticStruct();
}
