// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MuksiBattleCameraDataAsset.h"
#include "BattleCameraManager.generated.h"

class APlayerController;
class ABattleCharacterBase;
class UCineCameraComponent;
class USceneComponent;
class USpringArmComponent;
class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;
class ABattleManager;
class UBattlePhaseTaskContext;
class UMuksiBattleCameraDataAsset;
struct FMuksiBattleCameraData;


DECLARE_MULTICAST_DELEGATE(FOnActionCameraPreArrival);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleCameraMoveFinished, EBattleCameraMode);

/**
 * 카메라의 현재 동작 상태
 */
UENUM(BlueprintType)
enum class EBattleCameraMode : uint8
{
	Overview,			// 전장 전체 화면
	Targeting,			// Targeting Phase 시 카메라
	CharacterFocus,		// 캐릭터 선택 화면
	ActionPresentation, // 공격 직전 연출 화면
	Attack				// 공격 연출 화면
};

/**
 * 캐릭터 선택 카메라 설정
 *
 * BattleCameraManager가 기본 설정을 가지고 있고,
 * 각 캐릭터가 필요할 경우 이 값을 덮어쓴다.
 */
USTRUCT(BlueprintType)
struct FCharacterCameraFocusSettings
{
	GENERATED_BODY()

	/**
	 * 캐릭터별 카메라 설정을 사용할지 여부
	 *
	 * false이면 BattleCameraManager의 기본 설정을 사용한다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Camera"
	)
	bool bOverrideCameraSettings = false;

	/**
	 * 캐릭터 방향을 기준으로 한 카메라 상대 회전
	 *
	 * Pitch: 위아래 각도
	 * Yaw: 캐릭터를 기준으로 좌우 어느 방향에서 볼지
	 * Roll: 화면 기울기
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Camera",
		meta = (
			EditCondition = "bOverrideCameraSettings",
			EditConditionHides
		)
	)
	FRotator RelativeRotation =
		FRotator(-15.0f, 45.0f, 0.0f);

	/**
	 * CameraFocusPoint와 실제 카메라 사이 거리
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Camera",
		meta = (
			ClampMin = "0.0",
			EditCondition = "bOverrideCameraSettings",
			EditConditionHides
		)
	)
	float ArmLength = 550.0f;

	/**
	 * CineCamera의 초점거리
	 *
	 * 값이 클수록 화면이 확대되고 망원 느낌이 강해진다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Camera",
		meta = (
			ClampMin = "1.0",
			EditCondition = "bOverrideCameraSettings",
			EditConditionHides
		)
	)
	float FocalLength = 45.0f;
};

UCLASS()
class MUKSI_API ABattleCameraManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleCameraManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	

	// 공격 애니메이션 중 소켓 위치를 추적하는 카메라
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void FocusAttackCamera(ABattleCharacterBase* Character);

	// 탑뷰로 복귀
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void ReturnToOverview();

	// 이 Actor를 플레이어의 ViewTarget으로 설정
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void ActivateBattleCamera(float BlendTime = 0.0f);

	// 공격 도중 SpringArm 거리와 Focal Length 변경
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void SetAttackCameraSettings(
		float NewArmLength,
		float NewFocalLength,
		FVector NewSocketOffset,
		FRotator NewArmRotation
	);

	// 현재 추적만 중지
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void StopTrackingCamera();
	
	//Targeting Phase 카메라 전환-----------------------------------------------------------------------------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void EnterTargetingView();
	
	bool InitializeBattleFlow(ABattleManager* InBattleManager);
	
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle Camera|Targeting")
	TObjectPtr<AActor> TargetingCameraAnchor = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Targeting")
	float TargetingFocalLength = 35.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Targeting")
	float TargetingArmLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Targeting")
	FVector TargetingSocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Targeting")
	FRotator TargetingArmRotation = FRotator::ZeroRotator;
	
	UFUNCTION()
	void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager = nullptr;
	//------------------------------------------------------------------------------------------------------------------
	
	//BattleAction Present 카메라 전환------------------------------------------------------------------------------------
public:
	void FocusBattleActionPresentation(ABattleCharacterBase* Character);
	
	FOnActionCameraPreArrival OnActionCameraPreArrival;//카메라 이동 끝나기 전
	FOnBattleCameraMoveFinished OnCameraMoveFinished; //카메라 이동 끝난 직후
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Action Presentation")
	float ActionPresentationFocalLength = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Action Presentation")
	float ActionPresentationArmLength = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Battle Camera|Action Presentation")
	float ActionPresentationPreArrivalDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Action Presentation")
	FVector ActionPresentationSocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Action Presentation")
	FRotator ActionPresentationArmRotation = FRotator::ZeroRotator;
	
	
	//카메라 도착 지점 오차값(클 수록 미리 도착했다고 연락함)
	UPROPERTY(EditAnywhere, Category = "Battle Camera|Interpolation")
	float LocationCompleteTolerance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Battle Camera|Interpolation")
	float RotationCompleteTolerance = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Battle Camera|Interpolation")
	float ValueCompleteTolerance = 0.1f;
	
private:
	void CheckCameraMoveFinished();
	
	bool bActionCameraPreArrivalTriggered = false;
	bool bWaitingForCameraMoveCompletion = false;
	//------------------------------------------------------------------------------------------------------------------
	
protected:
	void SetCameraTarget(
		const FVector& NewLocation,
		const FRotator& NewRotation,
		float NewFocalLength,
		float NewArmLength
	);

protected:
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Battle Camera"
	)
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Battle Camera"
	)
	TObjectPtr<USpringArmComponent> CameraSpringArm = nullptr;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Battle Camera"
	)
	TObjectPtr<UCineCameraComponent> BattleCamera = nullptr;

protected:
	// 위치 보간 속도
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Battle Camera|Interpolation",
		meta = (ClampMin = "0.0")
	)
	float LocationInterpSpeed = 5.0f;

	// 회전 보간 속도
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Battle Camera|Interpolation",
		meta = (ClampMin = "0.0")
	)
	float RotationInterpSpeed = 5.0f;

	// SpringArm 길이 보간 속도
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Battle Camera|Interpolation",
		meta = (ClampMin = "0.0")
	)
	float ArmLengthInterpSpeed = 5.0f;

	// Focal Length 보간 속도
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Battle Camera|Interpolation",
		meta = (ClampMin = "0.0")
	)
	float FocalLengthInterpSpeed = 5.0f;

	//캐릭터 클릭 화면 Focus----------------------------------------------------------------------------------------------
public:
	// 캐릭터 클릭 시 사용하는 고정 포커스 카메라
	UFUNCTION(BlueprintCallable, Category = "Battle Camera")
	void FocusCharacter(ABattleCharacterBase* Character);
	
protected:
	// 캐릭터 클릭 화면 Focal Length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus", meta = (ClampMin = "1.0"))
	float CharacterFocusFocalLength = 35.0f;

	// 캐릭터 클릭 화면 SpringArm 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus", meta = (ClampMin = "1.0"))
	float CharacterFocusArmLength = 0.0f;

	// 공격 카메라 기본 Focal Length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus", meta = (ClampMin = "1.0"))
	float AttackFocalLength = 35.0f;

	// 공격 카메라 기본 SpringArm 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus", meta = (ClampMin = "1.0"))
	float AttackArmLength = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus")
	FVector CharacterFocusSocketOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Camera|Character Click Focus")
	FRotator CharacterFocusArmRotation = FRotator::ZeroRotator;

	//------------------------------------------------------------------------------------------------------------------
	
protected:
	// 게임 시작 시 탑뷰 위치
	FVector OverviewCameraLocation = FVector::ZeroVector;

	// 게임 시작 시 탑뷰 회전
	FRotator OverviewCameraRotation = FRotator::ZeroRotator;

	// 게임 시작 시 SpringArm 길이
	float OverviewArmLength = 0.0f;

	// 게임 시작 시 Focal Length
	float OverviewFocalLength = 35.0f;

protected:
	// 카메라가 이동할 목표 위치
	FVector TargetCameraLocation = FVector::ZeroVector;

	// 카메라가 이동할 목표 회전
	FRotator TargetCameraRotation = FRotator::ZeroRotator;

	// SpringArm 목표 길이
	float TargetArmLength = 0.0f;

	// 목표 Focal Length
	float TargetFocalLength = 35.0f;

	// SpringArm 목표 SocketOffset
	FVector TargetSocketOffset = FVector::ZeroVector;

	// SpringArm 목표 상대 회전
	FRotator TargetArmRelativeRotation = FRotator::ZeroRotator;

protected:
	// 공격 애니메이션 중 추적할 SceneComponent
	UPROPERTY()
	TObjectPtr<USceneComponent> TrackingCameraComponent = nullptr;

	bool bTrackingCameraComponent = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Camera")
	EBattleCameraMode CameraMode = EBattleCameraMode::Overview;

	UPROPERTY()
	TObjectPtr<APlayerController> CachedPlayerController = nullptr;
	
	
public:
	const FMuksiBattleCameraData* FindBattleCameraData(FName CameraKey) const;
	void CollectBattleCameraAssetPaths(const TSet<FName>& CameraKeys, TArray<FSoftObjectPath>& OutAssetPaths) const;
	void PlayBattleCameraSequence(FName CameraKey, ABattleCharacterBase* Attacker, ABattleCharacterBase* Target, FSimpleDelegate OnFinished = FSimpleDelegate());

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Camera|Sequence")
	TArray<TObjectPtr<UMuksiBattleCameraDataAsset>> BattleCameraDataAssets;

public:
	//공격 애니메이션 실행 Sequencer 작용
	UFUNCTION(BlueprintCallable, Category = "Battle Camera|Sequence")
	void PlayAttackCameraSequence(
		ULevelSequence* CameraSequence,
		ABattleCharacterBase* Attacker,
		ABattleCharacterBase* Target
	);
protected:
	UPROPERTY(EditInstanceOnly, Category = "Battle Camera|Sequence")
	TObjectPtr<ALevelSequenceActor> AttackSequenceActor = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Battle Camera|Sequence")
	TObjectPtr<AActor> AttackSequenceOrigin = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> ActiveSequencePlayer = nullptr;

	bool bOwnsAttackSequenceActor = false;
	EBattleCameraBlendOutMode ActiveSequenceBlendOutMode = EBattleCameraBlendOutMode::Legacy;
	float ActiveSequenceBlendOutDuration = 0.2f;
	float ActiveSequenceBlendOutExponent = 2.0f;

	bool bCameraPOVBlendActive = false;
	float CameraPOVBlendElapsedTime = 0.0f;
	float CameraPOVBlendDuration = 0.2f;
	float CameraPOVBlendExponent = 2.0f;
	FVector CameraPOVBlendStartLocation = FVector::ZeroVector;
	FQuat CameraPOVBlendStartRotation = FQuat::Identity;
	float CameraPOVBlendStartFocalLength = 35.0f;
	FVector CameraPOVBlendTargetLocation = FVector::ZeroVector;
	FQuat CameraPOVBlendTargetRotation = FQuat::Identity;
	FVector OverviewOrbitCenter = FVector::ZeroVector;

	UFUNCTION()
	void HandleAttackCameraSequenceFinished();
	
	void StopAttackCameraSequence();
	void ReturnToOverviewImmediately();
	void StartCameraPOVBlendToOverview(float BlendDuration, float BlendExponent);
	void StartOverviewOrbitBlendToOverview(float BlendDuration, float BlendExponent);
	void UpdateCameraPOVBlend(float DeltaTime);
	void UpdateOverviewOrbitBlend(float DeltaTime);
	void FinishCameraPOVBlend();

private:
	void WaitForBattleCameraEnd(FSimpleDelegate OnFinished);
	void FinishBattleCameraWait();
	void CancelBattleCameraWait();

	FSimpleDelegate CameraPlaybackFinishedDelegate;
	bool bWaitingForSequenceEnd = false;
};
