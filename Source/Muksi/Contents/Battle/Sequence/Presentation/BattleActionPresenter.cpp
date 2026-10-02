// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Sequence/Presentation/BattleActionPresenter.h"

#include "BattleSkillInfoActor.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

bool UBattleActionPresenter::Initialize(ABattleSequenceManager* InSequenceManager,
                                        ABattleCameraManager* InCameraManager,
                                        TSubclassOf<ABattleSkillInfoActor> InSkillInfoActorClass)
{
	if (!IsValid(InSequenceManager) || !IsValid(InCameraManager))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[BattleActionPresenter] "
				"Failed to initialize. SequenceManager=%s CameraManager=%s"
			),
			*GetNameSafe(InSequenceManager),
			*GetNameSafe(InCameraManager)
		);

		return false;
	}

	SequenceManager = InSequenceManager;
	SkillInfoActorClass = InSkillInfoActorClass;
	CameraManager = InCameraManager;
	CameraManager->OnActionCameraPreArrival.AddUObject(this, &UBattleActionPresenter::HandleCameraPreArrival);
	CameraManager->OnCameraMoveFinished.AddUObject(this, &UBattleActionPresenter::HandleCameraMoveFinished);

	return true;
}

void UBattleActionPresenter::Shutdown()
{
	if (IsValid(CameraManager))
	{
		CameraManager->OnActionCameraPreArrival.RemoveAll(this);
		CameraManager->OnCameraMoveFinished.RemoveAll(this);
	}

	bPresentationRunning = false;

	CameraManager = nullptr;
	SequenceManager = nullptr;
}

bool UBattleActionPresenter::StartPresentation(const FBattleAction& BattleAction)
{
	if (bPresentationRunning)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleActionPresenter] Presentation is already running.")
		);

		return false;
	}

	if (!IsValid(BattleAction.Attacker.Get()) || !IsValid(BattleAction.Card.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[BattleActionPresenter] Invalid BattleAction. "
				"Attacker=%s Card=%s"
			),
			*GetNameSafe(BattleAction.Attacker.Get()),
			*GetNameSafe(BattleAction.Card.Get())
		);

		return false;
	}

	CurrentAction = BattleAction;
	bPresentationRunning = true;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[BattleActionPresenter] Start Presentation. "
			"Attacker=%s Card=%s"
		),
		*GetNameSafe(CurrentAction.Attacker.Get()),
		*GetNameSafe(CurrentAction.Card.Get())
	);

	StartCameraPresentation();
	return true;
}

void UBattleActionPresenter::StartCameraPresentation()
{
	if (!IsValid(CameraManager) || !IsValid(CurrentAction.Attacker.Get()))
	{
		return;
	}

	CameraManager->FocusBattleActionPresentation(CurrentAction.Attacker.Get());
}

void UBattleActionPresenter::SpawnSkillInfoActor()
{
	if (!IsValid(SequenceManager) ||
		!SkillInfoActorClass ||
		!IsValid(CurrentAction.Attacker.Get()))
	{
		return;
	}

	UWorld* World = SequenceManager->GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	// 혹시 기존 Actor가 남아있으면 제거
	if (IsValid(SkillInfoActor))
	{
		SkillInfoActor->Destroy();
		SkillInfoActor = nullptr;
	}

	ABattleCharacterBase* Attacker = CurrentAction.Attacker.Get();
	USceneComponent* SkillInfoAnchor = Attacker->GetSkillInfoAnchorComponent();
	
	if (!IsValid(SkillInfoAnchor))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[BattleActionPresenter] "
				"SkillInfoAnchorComponent is invalid. Attacker=%s"
			),
			*GetNameSafe(Attacker)
		);

		return;
	}

	const FTransform SpawnTransform = SkillInfoAnchor->GetComponentTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Attacker;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SkillInfoActor = World->SpawnActor<ABattleSkillInfoActor>(SkillInfoActorClass, SpawnTransform, SpawnParams);

	if (!IsValid(SkillInfoActor))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[BattleActionPresenter] Failed to spawn SkillInfoActor.")
		);

		return;
	}

	SkillInfoActor->SetBattleAction(CurrentAction);
	SkillInfoActor->PlayShowPresentation();

	SkillInfoActor->OnShowFinished.AddUObject(this, &UBattleActionPresenter::HandleSkillInfoShowFinished);
}

void UBattleActionPresenter::HandleCameraPreArrival()
{
	if (!bPresentationRunning)
	{
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleActionPresenter] " "Action Camera Pre Arrival")
	);
	
	SpawnSkillInfoActor();
}

void UBattleActionPresenter::HandleCameraMoveFinished(EBattleCameraMode FinishedMode)
{
	if (!bPresentationRunning)
	{
		return;
	}

	if (FinishedMode != EBattleCameraMode::ActionPresentation)
	{
		return;
	}
	
	//카메라 이동 끝난 상태 받기
	
}

void UBattleActionPresenter::HandleSkillInfoShowFinished()
{
	FinishPresentation();
}

void UBattleActionPresenter::FinishPresentation()
{
	if (!bPresentationRunning)
	{
		return;
	}

	// 먼저 false로 내려야 함.
	// SequenceManager에 완료를 넘긴 뒤 다음 Action이 시작될 수도 있기 때문.
	bPresentationRunning = false;

	// 이번 Presentation에서 생성한 Actor 정리
	if (IsValid(SkillInfoActor))
	{
		SkillInfoActor->Destroy();
		SkillInfoActor = nullptr;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleActionPresenter] Presentation Finished")
	);

	if (IsValid(SequenceManager))
	{
		SequenceManager->NotifyBattleActionPresentationFinished();
	}
}
