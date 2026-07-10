#include "MuksiBattleAnimationComponent.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimInstance.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationDataAsset.h"

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
	if (CachedMeshComponent)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	CachedMeshComponent = Owner->FindComponentByClass<USkeletalMeshComponent>();

	if (!CachedMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] SkeletalMeshComponent not found. Owner=%s"),*GetNameSafe(Owner));
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

bool UMuksiBattleAnimationComponent::PlayBattleAnimation(const FName& AnimKey)
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

	const float PlayLength = AnimInstance->Montage_Play(Montage);

	if (PlayLength <= 0.f)
	{
		CurrentMontage = nullptr;
		UE_LOG(LogTemp, Error, TEXT("[BattleAnimationComponent] Montage_Play failed. Montage=%s"),
			*GetNameSafe(Montage));
		return false;
	}

	return true;
}

void UMuksiBattleAnimationComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CurrentMontage)
	{
		return;
	}

	CurrentMontage = nullptr;

	if (CachedMeshComponent)
	{
		if (UAnimInstance* AnimInstance = CachedMeshComponent->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UMuksiBattleAnimationComponent::HandleMontageEnded);
		}
	}

	OnBattleAnimationFinished.Broadcast(bInterrupted);
}

void UMuksiBattleAnimationComponent::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (NotifyKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleAnimationComponent] BattleExecutionNotifyKey is None."));
		return;
	}

	OnBattleExecutionNotify.Broadcast(NotifyKey);
}