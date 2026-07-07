#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include"MuksiTypes/MuksiWeaponTypes.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationKeys.h"
#include "MuksiBattleAnimationComponent.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;
class UMuksiBattleAnimationDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuksiBattleAnimationFinished, bool, bInterrupted);

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
	bool PlayBattleAnimation(const FName& AnimKey);

	UFUNCTION(BlueprintPure, Category = "Battle Animation")
	UAnimMontage* FindMontage(const FName& AnimKey) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> CachedMeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CurrentMontage;

private:
	void CacheMeshComponent();
	void ApplyWeaponTypeToAnimInstance();

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};