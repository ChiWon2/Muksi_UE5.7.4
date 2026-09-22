#include "MuksiBattleAnimationComponent.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimInstance.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

UMuksiBattleAnimationComponent::UMuksiBattleAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiBattleAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheMeshComponent();
	ApplyWeaponTypeToAnimInstance();
}

void UMuksiBattleAnimationComponent::CacheMeshComponent()
{
	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(GetOwner());
	if (!BattleCharacter)
	{
		CachedMeshComponent = nullptr;
		return;
	}

	// Always follow the character's authoritative MeshComponent pointer.
	// Simulation characters replace that pointer with a runtime-cloned source mesh
	// after BeginPlay, so a one-time FindComponentByClass cache can become stale.
	USkeletalMeshComponent* CurrentMeshComponent = BattleCharacter->GetMeshComponent();
	if (CachedMeshComponent != CurrentMeshComponent)
	{
		CachedMeshComponent = CurrentMeshComponent;
	}

	if (!CachedMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] SkeletalMeshComponent not found. Owner=%s"), *GetNameSafe(BattleCharacter));
	}
}

void UMuksiBattleAnimationComponent::PlayAnimMontage(EMuksiWeaponTypes InWeaponType, const FName& AnimKey)
{
	SetWeaponType(InWeaponType);
	PlayBattleAnimation(AnimKey);
}

void UMuksiBattleAnimationComponent::SetWeaponType(EMuksiWeaponTypes InWeaponType)
{
	CurrentWeaponType = InWeaponType;
	ApplyWeaponTypeToAnimInstance();
}

void UMuksiBattleAnimationComponent::SetCharacterState(EMuksiBattleCharacterState NewState)
{
	CacheMeshComponent();

	if (!CachedMeshComponent)
		return;

	UMuksiBattleAnimInstance* BattleAnimInstance = Cast<UMuksiBattleAnimInstance>(CachedMeshComponent->GetAnimInstance());
	if (!BattleAnimInstance)
		return;

	BattleAnimInstance->CharacterState = NewState;
}

void UMuksiBattleAnimationComponent::StopCurrentMontage(float BlendOutTime)
{
	CacheMeshComponent();

	if (!CachedMeshComponent || !CurrentMontage)
		return;

	UAnimInstance* AnimInstance = CachedMeshComponent->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Stop(FMath::Max(0.0f, BlendOutTime), CurrentMontage);
}

void UMuksiBattleAnimationComponent::ApplyWeaponTypeToAnimInstance()
{
	CacheMeshComponent();

	if (!CachedMeshComponent)
	{
		return;
	}

	UMuksiBattleAnimInstance* BattleAnimInstance = Cast<UMuksiBattleAnimInstance>(CachedMeshComponent->GetAnimInstance());

	if (!BattleAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] AnimInstance is not UMuksiBattleAnimInstance. Owner=%s"),*GetNameSafe(GetOwner()));
		return;
	}

	BattleAnimInstance->CurrentWeaponType = CurrentWeaponType;
}

UAnimMontage* UMuksiBattleAnimationComponent::FindMontage(const FName& AnimKey) const
{
	if (!AnimationData)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] AnimationData is null. Owner=%s"),
			*GetNameSafe(GetOwner()));
		return nullptr;
	}

	return AnimationData->FindMontage(AnimKey, CurrentWeaponType);
}

bool UMuksiBattleAnimationComponent::PlayBattleAnimation(const FName& AnimKey, float PlayRate)
{
	CacheMeshComponent();

	if (!CachedMeshComponent)
	{
		return false;
	}

	if (AnimKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] AnimKey is None."));
		return false;
	}

	UAnimInstance* AnimInstance = CachedMeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] AnimInstance is null. Owner=%s"),
			*GetNameSafe(GetOwner()));
		return false;
	}

	UAnimMontage* Montage = FindMontage(AnimKey);
	if (!Montage)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] Montage not found. WeaponType=%d AnimKey=%s"),
			static_cast<int32>(CurrentWeaponType),
			*AnimKey.ToString());
		return false;
	}

	AnimInstance->OnMontageEnded.RemoveDynamic(this, &UMuksiBattleAnimationComponent::HandleMontageEnded);
	AnimInstance->OnMontageEnded.AddDynamic(this, &UMuksiBattleAnimationComponent::HandleMontageEnded);

	CurrentMontage = Montage;
	CurrentAnimKey = AnimKey;

	const float PlayLength = AnimInstance->Montage_Play(Montage, PlayRate);

	if (PlayLength <= 0.f)
	{
		CurrentMontage = nullptr;
		CurrentAnimKey = NAME_None;
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] Montage_Play failed. Montage=%s"),
			*GetNameSafe(Montage));
		return false;
	}

	return true;
}

void UMuksiBattleAnimationComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const bool bWasCurrentMontage = Montage == CurrentMontage;

	if (bWasCurrentMontage)
	{
		CurrentMontage = nullptr;
		CurrentAnimKey = NAME_None;
	}

	if (bWasCurrentMontage)
	{
		if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
		{
			if (ABattleCameraManager* CameraManager = ManagerSubsystem->GetManager<ABattleCameraManager>())
				CameraManager->ReturnToOverview();
		}
	}

	OnBattleAnimationFinished.Broadcast(Montage, bInterrupted);
}

bool UMuksiBattleAnimationComponent::SetCurrentMontagePlayRate(float PlayRate)
{
	return SetMontagePlayRate(CurrentMontage, PlayRate);
}

bool UMuksiBattleAnimationComponent::SetMontagePlayRate(UAnimMontage* Montage, float PlayRate)
{
	CacheMeshComponent();

	if (!CachedMeshComponent || !Montage)
		return false;

	UAnimInstance* AnimInstance = CachedMeshComponent->GetAnimInstance();

	if (!AnimInstance)
		return false;

	AnimInstance->Montage_SetPlayRate(Montage, FMath::Max(0.0f, PlayRate));
	return true;
}

UAnimMontage* UMuksiBattleAnimationComponent::GetCurrentMontage() const
{
	return CurrentMontage;
}

FName UMuksiBattleAnimationComponent::GetCurrentAnimKey() const
{
	return CurrentAnimKey;
}

void UMuksiBattleAnimationComponent::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (NotifyKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] BattleExecutionNotifyKey is None."));
		return;
	}

	ABattleCharacterBase* NotifySource = Cast<ABattleCharacterBase>(GetOwner());

	OnBattleExecutionNotify.Broadcast(NotifyKey);
	OnBattleExecutionNotifyWithSource.Broadcast(NotifySource, NotifyKey);
	OnBattleExecutionNotifyWithSourceAndAnimKey.Broadcast(NotifySource, NotifyKey, CurrentAnimKey);
}

bool UMuksiBattleAnimationComponent::JumpCurrentMontageToSection(const FName& SectionName)
{
	return JumpMontageToSection(CurrentMontage, SectionName);
}

bool UMuksiBattleAnimationComponent::JumpMontageToSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (SectionName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] SectionName is None."));
		return false;
	}

	CacheMeshComponent();

	if (!CachedMeshComponent)
		return false;

	UAnimInstance* AnimInstance = CachedMeshComponent->GetAnimInstance();

	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] AnimInstance is null. Owner=%s"), *GetNameSafe(GetOwner()));
		return false;
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] Montage is null. Owner=%s"), *GetNameSafe(GetOwner()));
		return false;
	}

	if (!AnimInstance->Montage_IsActive(Montage))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] Montage is not active. Montage=%s"), *GetNameSafe(Montage));
		return false;
	}

	const int32 SectionIndex = Montage->GetSectionIndex(SectionName);

	if (SectionIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] Section not found. Montage=%s Section=%s"), *GetNameSafe(Montage), *SectionName.ToString());
		return false;
	}

	AnimInstance->Montage_JumpToSection(SectionName, Montage);

	UE_LOG(LogTemp, Log, TEXT("[BattleAnimationComponent] Jumped to Montage Section. Montage=%s Section=%s"), *GetNameSafe(Montage), *SectionName.ToString());

	return true;
}
