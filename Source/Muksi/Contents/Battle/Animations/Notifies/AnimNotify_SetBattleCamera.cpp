// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Animations/Notifies/AnimNotify_SetBattleCamera.h"

#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Components/SkeletalMeshComponent.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

void UAnimNotify_SetBattleCamera::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(
		MeshComp,
		Animation,
		EventReference
	);
	UE_LOG(LogTemp, Error, TEXT("Notify Test!!!!!!!!!!!!!!!!!!!!!!!"));
	if (!IsValid(MeshComp))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AnimNotify_SetBattleCamera: MeshComp is invalid")
		);

		return;
	}

	if (!IsValid(CameraSequence))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AnimNotify_SetBattleCamera: CameraSequence is not set")
		);

		return;
	}
	ABattleCharacterBase* Attacker =
		Cast<ABattleCharacterBase>(
			MeshComp->GetOwner()
		);

	if (!IsValid(Attacker))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"AnimNotify_SetBattleCamera: "
				"Mesh owner is not ABattleCharacterBase"
			)
		);

		return;
	}

	UMuksiWorldManagerSubsystem* ManagerSubsystem =
		UMuksiWorldManagerSubsystem::Get(MeshComp);

	if (!IsValid(ManagerSubsystem))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"AnimNotify_SetBattleCamera: "
				"ManagerSubsystem is invalid"
			)
		);

		return;
	}

	ABattleCameraManager* CameraManager =
		ManagerSubsystem->GetManager<ABattleCameraManager>();

	if (!IsValid(CameraManager))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"AnimNotify_SetBattleCamera: "
				"BattleCameraManager is invalid"
			)
		);

		return;
	}

	CameraManager->PlayAttackCameraSequence(
		CameraSequence,
		Attacker
	);
}

FString UAnimNotify_SetBattleCamera::GetNotifyName_Implementation() const
{
	if (IsValid(CameraSequence))
	{
		return FString::Printf(
			TEXT("Camera: %s"),
			*CameraSequence->GetName()
		);
	}

	return TEXT("Play Battle Camera Sequence");
}
