// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Sequence/Presentation/BattleActionGroupController.h"

#include "BattleSkillInfoActor.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

bool UBattleActionGroupController::Initialize(ABattleSequenceManager* InSequenceManager, ABattleCameraManager* InCameraManager, TSubclassOf<ABattleSkillInfoActor> InSkillInfoActorClass)
{
	if (!IsValid(InSequenceManager) || !IsValid(InCameraManager))
	{
		return false;
	}

	SequenceManager = InSequenceManager;
	CameraManager = InCameraManager;

	bPresentationRunning = false;
	bWaitingForStartCamera = false;
	
	SkillInfoActorClass = InSkillInfoActorClass;
	bWaitingForSkillInfo = false;

	CameraManager->OnActionCameraPreArrival.RemoveAll(this);
	CameraManager->OnActionCameraPreArrival.AddUObject(this, &UBattleActionGroupController::HandleCameraMoveFinished);

	return true;
}

void UBattleActionGroupController::Shutdown()
{
	if (IsValid(CameraManager))
	{
		CameraManager->OnActionCameraPreArrival.RemoveAll(this);
	}

	CurrentPresentationActionIndex = INDEX_NONE;
	
	bPresentationRunning = false;
	bWaitingForStartCamera = false;

	CurrentGroupActions.Reset();

	CameraManager = nullptr;
	SequenceManager = nullptr;
	
	ClearSkillInfoActor();
	ResetCurrentActionPresentationState();
}

void UBattleActionGroupController::PlayGroupStart(const TArray<FBattleAction>& InActions)
{
	if (!IsValid(SequenceManager) || bPresentationRunning)
	{
		return;
	}

	CurrentGroupActions = InActions;
	CurrentPresentationActionIndex = 0;

	bPresentationRunning = true;
	bWaitingForStartCamera = false;
	
	StartCurrentActionCamera();
}

void UBattleActionGroupController::PlayGroupEnd()
{
	if (!IsValid(SequenceManager) || bPresentationRunning)
	{
		return;
	}

	bPresentationRunning = true;

	UE_LOG(
		LogTemp, Warning,
		TEXT("[ActionGroup] End. ActionCount=%d"),
		CurrentGroupActions.Num()
	);

	// 실제 종료 연출을 넣으면 그 완료 시점으로 이동
	bPresentationRunning = false;
	CurrentGroupActions.Reset();

	SequenceManager->NotifyActionGroupEndFinished();
}

void UBattleActionGroupController::HandleCameraMoveFinished()
{
	if (!bPresentationRunning ||
		!bWaitingForStartCamera)
	{
		return;
	}

	bWaitingForStartCamera = false;

	UE_LOG(
		LogTemp, Warning,
		TEXT("[ActionGroup] Camera Finished. Index=%d"),
		CurrentPresentationActionIndex);

	SpawnCurrentSkillInfoActor();
}

void UBattleActionGroupController::FinishGroupStart()
{
	if (!bPresentationRunning)
	{
		return;
	}

	bWaitingForStartCamera = false;
	bPresentationRunning = false;

	if (IsValid(SequenceManager))
	{
		SequenceManager->NotifyActionGroupStartFinished();
	}
}

void UBattleActionGroupController::StartCurrentActionCamera()
{
	if (!bPresentationRunning)
	{
		return;
	}

	if (!CurrentGroupActions.IsValidIndex(CurrentPresentationActionIndex))
	{
		FinishGroupStart();
		return;
	}

	const FBattleAction& Action = CurrentGroupActions[CurrentPresentationActionIndex];

	if (!IsValid(Action.Attacker.Get()))
	{
		AdvancePresentationAction();
		return;
	}

	if (!IsValid(CameraManager))
	{
		SpawnCurrentSkillInfoActor();
		return;
	}

	bWaitingForStartCamera = true;
	CameraManager->FocusBattleActionPresentation(Action.Attacker.Get());
}

void UBattleActionGroupController::NotifySkillRevealDeceiveFinished()
{
	if (!bPresentationRunning || !bWaitingForDeceivePresentation)
	{
		return;
	}

	bSkillRevealDeceiveFinished = true;

	TryFinishDeceivePresentation();
}

void UBattleActionGroupController::SpawnCurrentSkillInfoActor()
{
	if (!bPresentationRunning)
	{
		return;
	}

	ClearSkillInfoActor();
	ResetCurrentActionPresentationState();

	if (!CurrentGroupActions.IsValidIndex(
			CurrentPresentationActionIndex))
	{
		FinishGroupStart();
		return;
	}

	const FBattleAction& Action = CurrentGroupActions[CurrentPresentationActionIndex];

	ABattleCharacterBase* Attacker = Action.Attacker.Get();

	UWorld* World = IsValid(SequenceManager)
		? SequenceManager->GetWorld()
		: nullptr;

	USceneComponent* Anchor = IsValid(Attacker)
		? Attacker->GetSkillInfoAnchorComponent()
		: nullptr;

	if (!IsValid(World) ||
		!IsValid(Anchor) ||
		!IsValid(Action.Card.Get()) ||
		!SkillInfoActorClass)
	{
		UE_LOG(
			LogTemp, Error,
			TEXT("[ActionGroup] Cannot spawn SkillInfoActor. Index=%d"),
			CurrentPresentationActionIndex);

		AdvancePresentationAction();
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Attacker;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SkillInfoActor = World->SpawnActor<ABattleSkillInfoActor>(
		SkillInfoActorClass,
		Anchor->GetComponentTransform(),
		SpawnParams);

	if (!IsValid(SkillInfoActor))
	{
		UE_LOG(
			LogTemp, Error,
			TEXT("[ActionGroup] SkillInfoActor spawn failed"));

		AdvancePresentationAction();
		return;
	}

	SkillInfoActor->OnShowFinished.RemoveAll(this);
	SkillInfoActor->OnShowFinished.AddUObject(this, &UBattleActionGroupController::HandleSkillInfoShowFinished);
	SkillInfoActor->OnDeceiveRevealStarted.AddUObject(this, &UBattleActionGroupController::HandleSkillInfoDeceiveRevealStarted);

	bWaitingForSkillInfo = true;

	
	
	bWaitingForDeceivePresentation = IsValid(Action.Card->GetDeceivedCard());

	bSkillInfoDeceiveFinished = false;
	// 일반 스킬은 SkillRevealPanel의 변초 공개를 기다리지 않음
	bSkillRevealDeceiveFinished = !bWaitingForDeceivePresentation;

	SkillInfoActor->PlayShowPresentation(Action);
}

void UBattleActionGroupController::HandleSkillInfoShowFinished()
{
	if (!bPresentationRunning ||
		!bWaitingForSkillInfo)
	{
		return;
	}

	bWaitingForSkillInfo = false;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ActionGroup] SkillInfo Finished. Index=%d"),
		CurrentPresentationActionIndex
	);

	if (bWaitingForDeceivePresentation)
	{
		bSkillInfoDeceiveFinished = true;

		TryFinishDeceivePresentation();
		return;
	}

	// 일반 스킬은 Actor 연출만 끝나면 진행
	FinishCurrentActionPresentation();
}

void UBattleActionGroupController::AdvancePresentationAction()
{
	if (!bPresentationRunning)
	{
		return;
	}

	++CurrentPresentationActionIndex;
	
	if (!CurrentGroupActions.IsValidIndex(CurrentPresentationActionIndex))
	{
		FinishGroupStart();
		return;
	}
	StartCurrentActionCamera();
}



void UBattleActionGroupController::ClearSkillInfoActor()
{
	bWaitingForSkillInfo = false;

	if (IsValid(SkillInfoActor))
	{
		SkillInfoActor->OnShowFinished.RemoveAll(this);
		SkillInfoActor->OnDeceiveRevealStarted.RemoveAll(this);

		SkillInfoActor->Destroy();
	}

	SkillInfoActor = nullptr;
}

void UBattleActionGroupController::HandleSkillInfoDeceiveRevealStarted()
{
	if (!bPresentationRunning)
	{
		return;
	}

	RequestSkillRevealDeceivePresentation();
}

void UBattleActionGroupController::RequestSkillRevealDeceivePresentation()
{
	if (!bPresentationRunning || !bWaitingForDeceivePresentation)
	{
		return;
	}

	if (!CurrentGroupActions.IsValidIndex(CurrentPresentationActionIndex))
	{
		return;
	}

	// UI가 연결되지 않았다면 UI 완료로 처리
	if (!OnSkillRevealDeceiveRequested.IsBound())
	{
		NotifySkillRevealDeceiveFinished();
		return;
	}

	const FBattleAction& Action = CurrentGroupActions[CurrentPresentationActionIndex];

	OnSkillRevealDeceiveRequested.Broadcast(Action);
}

void UBattleActionGroupController::TryFinishDeceivePresentation()
{
	if (!bPresentationRunning ||
		!bWaitingForDeceivePresentation)
	{
		return;
	}

	if (!bSkillInfoDeceiveFinished ||
		!bSkillRevealDeceiveFinished)
	{
		return;
	}

	FinishCurrentActionPresentation();
}

void UBattleActionGroupController::FinishCurrentActionPresentation()
{
	if (!bPresentationRunning)
	{
		return;
	}

	ClearSkillInfoActor();
	ResetCurrentActionPresentationState();

	AdvancePresentationAction();
}

void UBattleActionGroupController::ResetCurrentActionPresentationState()
{
	bWaitingForSkillInfo = false;
	bWaitingForDeceivePresentation = false;

	bSkillInfoDeceiveFinished = false;
	bSkillRevealDeceiveFinished = false;

}
