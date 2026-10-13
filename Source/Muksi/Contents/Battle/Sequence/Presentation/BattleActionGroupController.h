// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "BattleActionGroupController.generated.h"

class ABattleSequenceManager;

class ABattleSkillInfoActor;
class ABattleCameraManager;

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActionGroupSkillRevealDeceiveRequested, const FBattleAction&);
UCLASS()
class MUKSI_API UBattleActionGroupController : public UObject
{
	GENERATED_BODY()
public:
	bool Initialize(ABattleSequenceManager* InSequenceManager, ABattleCameraManager* InCameraManager, TSubclassOf<ABattleSkillInfoActor> InSkillInfoActorClass);
	void Shutdown();

	void PlayGroupStart(const TArray<FBattleAction>& InActions);
	void PlayGroupEnd();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SequenceManager;

	TArray<FBattleAction> CurrentGroupActions;

	bool bPresentationRunning = false;
	
	int32 CurrentPresentationActionIndex = INDEX_NONE;

	
	//카메라 연출---------------------------------------------------------------------------------------------------------
public:
	
private:
	
	UPROPERTY(Transient)
	TObjectPtr<ABattleCameraManager> CameraManager;

	bool bWaitingForStartCamera = false;

	void HandleCameraMoveFinished();
	void FinishGroupStart();
	
	void StartCurrentActionCamera();
	//------------------------------------------------------------------------------------------------------------------
	
	//BattleSkillInfo 연출-----------------------------------------------------------------------------------------------
public:
	// SkillRevealPanel에 현재 Action의 변초 공개를 요청
	FOnActionGroupSkillRevealDeceiveRequested OnSkillRevealDeceiveRequested;

	// SkillRevealPanel의 변초 공개가 끝났을 때 호출
	void NotifySkillRevealDeceiveFinished();
private:
	UPROPERTY(Transient)
	TSubclassOf<ABattleSkillInfoActor> SkillInfoActorClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSkillInfoActor> SkillInfoActor;

	bool bWaitingForSkillInfo = false;

	void SpawnCurrentSkillInfoActor();
	void HandleSkillInfoShowFinished();
	void AdvancePresentationAction();
	void ClearSkillInfoActor();
	void HandleSkillInfoDeceiveRevealStarted();
	void RequestSkillRevealDeceivePresentation();
	
	// 현재 Action이 변초 연출을 기다리고 있는지
	bool bWaitingForDeceivePresentation = false;

	// SkillInfoActor 연출 완료 여부
	bool bSkillInfoDeceiveFinished = false;

	// SkillRevealPanel 변초 공개 완료 여부
	bool bSkillRevealDeceiveFinished = false;

	// Actor와 UI가 모두 끝났는지 확인
	void TryFinishDeceivePresentation();

	// 현재 Action의 연출을 정리하고 다음 Action으로 진행
	void FinishCurrentActionPresentation();

	// 다음 Action을 위해 완료 상태 초기화
	void ResetCurrentActionPresentationState();
	//------------------------------------------------------------------------------------------------------------------
};
