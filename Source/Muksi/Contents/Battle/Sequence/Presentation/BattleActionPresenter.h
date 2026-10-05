// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "UObject/Object.h"
#include "BattleActionPresenter.generated.h"

class ABattleSequenceManager;
class ABattleCameraManager;
class ABattleSkillInfoActor;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillRevealDeceiveRequested, const FBattleAction&);

/**
 * 
 */
UCLASS()
class MUKSI_API UBattleActionPresenter : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSequenceManager* InSequenceManager, ABattleCameraManager* InCameraManager, TSubclassOf<ABattleSkillInfoActor> InSkillInfoActorClass);
	void Shutdown();
	
	bool StartPresentation(const FBattleAction& BattleAction);
	
	void StartCameraPresentation();
	
	void SpawnSkillInfoActor();
	
	void NotifySkillRevealDeceiveFinished();
	
	FOnSkillRevealDeceiveRequested OnSkillRevealDeceiveRequested;
private:
	//카메라 이동 끝나기 직전
	void HandleCameraPreArrival();
	//카메라 이동 끝난 상태
	void HandleCameraMoveFinished(EBattleCameraMode FinishedMode);
	
	void HandleSkillInfoShowFinished();
	
	void FinishPresentation();
	
	void TryFinishDeceivePresentation();
	
	void RequestSkillRevealDeceivePresentation();
	
	void HandleSkillInfoDeceiveRevealStarted();
	
	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SequenceManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCameraManager> CameraManager = nullptr;
	
	UPROPERTY(Transient)
	TSubclassOf<ABattleSkillInfoActor> SkillInfoActorClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSkillInfoActor> SkillInfoActor = nullptr;
	
	FBattleAction CurrentAction;

	bool bPresentationRunning = false;
	
	bool bSkillInfoDeceiveFinished = false;
	bool bSkillRevealDeceiveFinished = false;
	
	bool bWaitingForDeceivePresentation = false;
};
