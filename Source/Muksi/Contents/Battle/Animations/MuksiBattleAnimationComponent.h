#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include"MuksiTypes/MuksiWeaponTypes.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationKeys.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimInstance.h"
#include "MuksiBattleAnimationComponent.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;
class ABattleCharacterBase;
class UMuksiBattleAnimationDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMuksiBattleExecutionNotify,
	FName,
	NotifyKey
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMuksiBattleExecutionNotifyWithSource,
	ABattleCharacterBase*,
	NotifySource,
	FName,
	NotifyKey
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnMuksiBattleExecutionNotifyWithSourceAndAnimKey,
	ABattleCharacterBase*,
	NotifySource,
	FName,
	NotifyKey,
	FName,
	SourceAnimKey
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMuksiBattleAnimationFinished,
	UAnimMontage*,
	Montage,
	bool,
	bInterrupted
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiBattleAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMuksiBattleAnimationComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Animation")
	TObjectPtr<UMuksiBattleAnimationDataAsset> AnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Animation")
	EMuksiWeaponTypes CurrentWeaponType = EMuksiWeaponTypes::None;

	UPROPERTY(BlueprintAssignable, Category = "Battle Animation")
	FOnMuksiBattleAnimationFinished OnBattleAnimationFinished;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	void PlayAnimMontage(EMuksiWeaponTypes InWeaponType, const FName& AnimKey);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	void SetWeaponType(EMuksiWeaponTypes InWeaponType);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	void SetCharacterState(EMuksiBattleCharacterState NewState);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	void StopCurrentMontage(float BlendOutTime);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	bool PlayBattleAnimation(const FName& AnimKey, float PlayRate = 1.0f);

	UFUNCTION(BlueprintPure, Category = "Battle Animation")
	UAnimMontage* FindMontage(const FName& AnimKey) const;

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	bool JumpCurrentMontageToSection(const FName& SectionName);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	bool JumpMontageToSection(UAnimMontage* Montage, const FName& SectionName);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	bool SetCurrentMontagePlayRate(float PlayRate);

	UFUNCTION(BlueprintCallable, Category = "Battle Animation")
	bool SetMontagePlayRate(UAnimMontage* Montage, float PlayRate);

	UFUNCTION(BlueprintPure, Category = "Battle Animation")
	UAnimMontage* GetCurrentMontage() const;

	UFUNCTION(BlueprintPure, Category = "Battle Animation")
	FName GetCurrentAnimKey() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> CachedMeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CurrentMontage;

	UPROPERTY(Transient)
	FName CurrentAnimKey = NAME_None;

private:
	void CacheMeshComponent();
	void ApplyWeaponTypeToAnimInstance();

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY(BlueprintAssignable, Category = "Battle Animation")
	FOnMuksiBattleExecutionNotify OnBattleExecutionNotify;

	UPROPERTY(BlueprintAssignable, Category = "Battle Animation")
	FOnMuksiBattleExecutionNotifyWithSource OnBattleExecutionNotifyWithSource;

	UPROPERTY(BlueprintAssignable, Category = "Battle Animation")
	FOnMuksiBattleExecutionNotifyWithSourceAndAnimKey OnBattleExecutionNotifyWithSourceAndAnimKey;

	void HandleBattleExecutionNotify(FName NotifyKey);
};