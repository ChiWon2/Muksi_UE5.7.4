// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"

#include "CharacterCameraComponent.h"
#include "CineCameraComponent.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

ABattleCameraManager::ABattleCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot =
		CreateDefaultSubobject<USceneComponent>(
			TEXT("SceneRoot")
		);

	SetRootComponent(SceneRoot);

	CameraSpringArm =
		CreateDefaultSubobject<USpringArmComponent>(
			TEXT("CameraSpringArm")
		);

	CameraSpringArm->SetupAttachment(SceneRoot);

	// 기본 길이
	CameraSpringArm->TargetArmLength = 0.0f;

	// PlayerController 회전과 무관하게 직접 제어
	CameraSpringArm->bUsePawnControlRotation = false;

	// 카메라 충돌 테스트를 사용하지 않음
	// 필요하면 나중에 true로 변경
	CameraSpringArm->bDoCollisionTest = false;

	// SpringArm 자체 랙은 사용하지 않고
	// BattleCameraManager Tick에서 직접 보간
	CameraSpringArm->bEnableCameraLag = false;
	CameraSpringArm->bEnableCameraRotationLag = false;

	BattleCamera =
		CreateDefaultSubobject<UCineCameraComponent>(
			TEXT("BattleCamera")
		);
	BattleCamera->SetupAttachment(CameraSpringArm);
	
	BattleCamera->SetupAttachment(
		CameraSpringArm,
		USpringArmComponent::SocketName
	);
	
	BattleCamera->SetUsingAbsoluteLocation(false);
	BattleCamera->SetUsingAbsoluteRotation(false);
	BattleCamera->SetUsingAbsoluteScale(false);
	
	BattleCamera->SetRelativeLocation(FVector::ZeroVector);
	BattleCamera->SetRelativeRotation(FRotator::ZeroRotator);
}

void ABattleCameraManager::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayerController =
		UGameplayStatics::GetPlayerController(this, 0);

	// WorldManagerSubsystem 등록
	if (UWorld* World = GetWorld())
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem =
			World->GetSubsystem<UMuksiWorldManagerSubsystem>())
		{
			ManagerSubsystem->RegisterManager(this);
		}
	}

	if (!IsValid(BattleCamera) ||
		!IsValid(CameraSpringArm))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("BattleCamera or CameraSpringArm is invalid")
		);

		return;
	}

	// 현재 에디터에 배치된 상태를 탑뷰 상태로 저장
	OverviewCameraLocation = GetActorLocation();
	OverviewCameraRotation = GetActorRotation();
	OverviewArmLength = CameraSpringArm->TargetArmLength;
	OverviewFocalLength = BattleCamera->CurrentFocalLength;

	TargetCameraLocation = OverviewCameraLocation;
	TargetCameraRotation = OverviewCameraRotation;
	TargetArmLength = OverviewArmLength;
	TargetFocalLength = OverviewFocalLength;

	TargetSocketOffset = CameraSpringArm->SocketOffset;
	TargetArmRelativeRotation =
		CameraSpringArm->GetRelativeRotation();

	CameraMode = EBattleCameraMode::Overview;

	ActivateBattleCamera(0.0f);
}

void ABattleCameraManager::EndPlay(
	const EEndPlayReason::Type EndPlayReason
)
{
	if (UMuksiWorldManagerSubsystem* ManagerSubsystem =
		UMuksiWorldManagerSubsystem::Get(this))
	{
		if (!ManagerSubsystem->UnregisterManager(this))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT(
					"BattleCameraManager: "
					"Failed to unregister manager"
				)
			);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABattleCameraManager::Tick(
	float DeltaTime
)
{
	Super::Tick(DeltaTime);

	if (!IsValid(BattleCamera) ||
		!IsValid(CameraSpringArm))
	{
		return;
	}

	/*
	 * 공격 애니메이션 중에는
	 * AttackCameraComponent의 월드 Transform을 매 프레임 읽는다.
	 */
	if (bTrackingCameraComponent)
	{
		if (IsValid(TrackingCameraComponent))
		{
			const FTransform TrackingTransform =
				TrackingCameraComponent->GetComponentTransform();

			TargetCameraLocation =
				TrackingTransform.GetLocation();

			/*TargetCameraRotation =
				TrackingTransform.Rotator();*/
		}
		else
		{
			bTrackingCameraComponent = false;
			TrackingCameraComponent = nullptr;
		}
	}

	// Manager Actor 위치 보간
	const FVector NewLocation =
		FMath::VInterpTo(
			GetActorLocation(),
			TargetCameraLocation,
			DeltaTime,
			LocationInterpSpeed
		);

	// Manager Actor 회전 보간
	const FRotator NewRotation =
		FMath::RInterpTo(
			GetActorRotation(),
			TargetCameraRotation,
			DeltaTime,
			RotationInterpSpeed
		);

	SetActorLocationAndRotation(
		NewLocation,
		NewRotation
	);

	// SpringArm 길이 보간
	const float NewArmLength =
		FMath::FInterpTo(
			CameraSpringArm->TargetArmLength,
			TargetArmLength,
			DeltaTime,
			ArmLengthInterpSpeed
		);

	CameraSpringArm->TargetArmLength = NewArmLength;

	// SpringArm SocketOffset 보간
	const FVector NewSocketOffset =
		FMath::VInterpTo(
			CameraSpringArm->SocketOffset,
			TargetSocketOffset,
			DeltaTime,
			LocationInterpSpeed
		);

	CameraSpringArm->SocketOffset = NewSocketOffset;

	// SpringArm 상대 회전 보간
	const FRotator NewArmRotation =
		FMath::RInterpTo(
			CameraSpringArm->GetRelativeRotation(),
			TargetArmRelativeRotation,
			DeltaTime,
			RotationInterpSpeed
		);

	CameraSpringArm->SetRelativeRotation(NewArmRotation);

	// CineCamera Focal Length 보간
	const float NewFocalLength =
		FMath::FInterpTo(
			BattleCamera->CurrentFocalLength,
			TargetFocalLength,
			DeltaTime,
			FocalLengthInterpSpeed
		);

	BattleCamera->SetCurrentFocalLength(NewFocalLength);
}



void ABattleCameraManager::FocusCharacter(
	ABattleCharacterBase* Character
)
{
	if (!IsValid(Character))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FocusCharacter: Character is invalid")
		);

		return;
	}

	UCharacterCameraComponent* FocusComponent =
		Character->GetClickCameraFocusComponent();

	if (!IsValid(FocusComponent))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FocusCharacter: ClickCameraFocusComponent is invalid")
		);

		return;
	}

	StopTrackingCamera();

	CameraMode = EBattleCameraMode::CharacterFocus;

	const FTransform FocusTransform =
		FocusComponent->GetComponentTransform();

	TargetSocketOffset =
		FocusComponent->SocketOffset;

	TargetArmRelativeRotation =
		FocusComponent->ArmRotation;

	SetCameraTarget(
		FocusTransform.GetLocation(),
		FocusTransform.Rotator(),
		FocusComponent->FocalLength,
		FocusComponent->ArmLength
	);
}

void ABattleCameraManager::FocusAttackCamera(ABattleCharacterBase* Character)
{
	if (!IsValid(Character))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FocusAttackCamera: Character is invalid")
		);

		return;
	}

	USceneComponent* AttackCameraComponent =
		Character->GetAttackCameraComponent();

	if (!IsValid(AttackCameraComponent))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FocusAttackCamera: AttackCameraComponent is invalid")
		);

		return;
	}

	TrackingCameraComponent = AttackCameraComponent;
	bTrackingCameraComponent = true;

	CameraMode = EBattleCameraMode::Attack;

	const FTransform AttackCameraTransform =
		AttackCameraComponent->GetComponentTransform();

	TargetSocketOffset = FVector::ZeroVector;
	TargetArmRelativeRotation = FRotator::ZeroRotator;

	SetCameraTarget(
		AttackCameraTransform.GetLocation(),
		AttackCameraTransform.Rotator(),
		AttackFocalLength,
		AttackArmLength
	);
	
	//로그 테스트
	UE_LOG(
	LogTemp,
	Warning,
	TEXT("Focus Attack Camera: Character=%s Location=%s Rotation=%s"),
	*GetNameSafe(Character),
	*AttackCameraTransform.GetLocation().ToString(),
	*AttackCameraTransform.Rotator().ToString()
	);
}

void ABattleCameraManager::ReturnToOverview()
{
	StopTrackingCamera();

	CameraMode = EBattleCameraMode::Overview;

	TargetSocketOffset = FVector::ZeroVector;
	TargetArmRelativeRotation = FRotator::ZeroRotator;

	SetCameraTarget(
		OverviewCameraLocation,
		OverviewCameraRotation,
		OverviewFocalLength,
		OverviewArmLength
	);
}

void ABattleCameraManager::ActivateBattleCamera(float BlendTime)
{
	if (!IsValid(CachedPlayerController))
	{
		CachedPlayerController =
			UGameplayStatics::GetPlayerController(this, 0);
	}

	if (!IsValid(CachedPlayerController))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PlayerController is invalid")
		);

		return;
	}

	CachedPlayerController->SetViewTargetWithBlend(
		this,
		BlendTime
	);
}

void ABattleCameraManager::SetAttackCameraSettings(float NewArmLength, float NewFocalLength, FVector NewSocketOffset,
	FRotator NewArmRotation)
{
	TargetArmLength =
		FMath::Max(0.0f, NewArmLength);

	TargetFocalLength =
		FMath::Max(1.0f, NewFocalLength);

	TargetSocketOffset = NewSocketOffset;
	TargetArmRelativeRotation = NewArmRotation;
}

void ABattleCameraManager::StopTrackingCamera()
{
	bTrackingCameraComponent = false;
	TrackingCameraComponent = nullptr;
}

void ABattleCameraManager::SetCameraTarget(const FVector& NewLocation, const FRotator& NewRotation,
	float NewFocalLength, float NewArmLength)
{
	TargetCameraLocation = NewLocation;
	TargetCameraRotation = NewRotation;

	TargetFocalLength =
		FMath::Max(
			1.0f,
			NewFocalLength
		);

	TargetArmLength =
		FMath::Max(
			0.0f,
			NewArmLength
		);
}

void ABattleCameraManager::PlayAttackCameraSequence(ULevelSequence* CameraSequence, ABattleCharacterBase* Attacker)
{
	if (!IsValid(CameraSequence) ||
		!IsValid(Attacker) ||
		!IsValid(AttackSequenceActor) ||
		!IsValid(AttackSequenceOrigin))
	{
		return;
	}

	StopAttackCameraSequence();

	// 기존 부모가 있다면 분리
	AttackSequenceOrigin->DetachFromActor(
		FDetachmentTransformRules::KeepWorldTransform
	);

	// 공격자의 Actor Root에 부착
	AttackSequenceOrigin->AttachToActor(
		Attacker,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	// 공격자 기준 원점
	AttackSequenceOrigin->SetActorRelativeLocation(
		FVector::ZeroVector
	);

	AttackSequenceOrigin->SetActorRelativeRotation(
		FRotator::ZeroRotator
	);

	AttackSequenceActor->SetSequence(CameraSequence);

	ULevelSequencePlayer* SequencePlayer =
		AttackSequenceActor->GetSequencePlayer();

	if (!IsValid(SequencePlayer))
	{
		return;
	}

	ActiveSequencePlayer = SequencePlayer;

	ActiveSequencePlayer->OnFinished.RemoveDynamic(
		this,
		&ThisClass::HandleAttackCameraSequenceFinished
	);

	ActiveSequencePlayer->OnFinished.AddDynamic(
		this,
		&ThisClass::HandleAttackCameraSequenceFinished
	);

	ActiveSequencePlayer->SetPlaybackPosition(
		FMovieSceneSequencePlaybackParams(
			0.0f,
			EUpdatePositionMethod::Jump
		)
	);

	ActiveSequencePlayer->Play();
}

void ABattleCameraManager::HandleAttackCameraSequenceFinished()
{
	if (IsValid(ActiveSequencePlayer))
	{
		ActiveSequencePlayer->OnFinished.RemoveDynamic(
			this,
			&ThisClass::HandleAttackCameraSequenceFinished
		);
	}

	ActiveSequencePlayer = nullptr;

	ReturnToOverview();
}

void ABattleCameraManager::StopAttackCameraSequence()
{
	if (IsValid(ActiveSequencePlayer))
	{
		ActiveSequencePlayer->OnFinished.RemoveDynamic(
			this,
			&ThisClass::HandleAttackCameraSequenceFinished
		);

		if (ActiveSequencePlayer->IsPlaying())
		{
			ActiveSequencePlayer->Stop();
		}
	}

	ActiveSequencePlayer = nullptr;

	if (IsValid(AttackSequenceOrigin))
	{
		AttackSequenceOrigin->DetachFromActor(
			FDetachmentTransformRules::KeepWorldTransform
		);
	}
}



