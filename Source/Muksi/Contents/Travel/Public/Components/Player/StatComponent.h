#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/CharacterStatTypes.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatChanged);

class UCharacterStatDataAsset;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnStatChanged OnStatChanged;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<UCharacterStatDataAsset> StatDataAsset = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FBaseStat BaseStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FWeaponMasteryStat WeaponMasteryStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FSpecialStat SpecialStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FTravelStat TravelStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FResourceStat ResourceStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FCombatStat CombatStat;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void InitializeStat();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void RecalculateStats();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void RefreshStats();

	UFUNCTION(BlueprintCallable, Category = "Stat|Resource")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat|Resource")
	void RecoverInternalEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat|Resource")
	bool UseInternalEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat|Resource")
	void TakeDamage(float Damage);

	UFUNCTION(BlueprintCallable, Category = "Stat|Growth")
	void AddLightnessSkill(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|Growth")
	void AddInternalEnergy(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|Growth")
	void AddVitality(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|WeaponMastery")
	void AddSwordMastery(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|WeaponMastery")
	void AddSpearMastery(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|WeaponMastery")
	void AddHiddenWeaponMastery(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|WeaponMastery")
	void AddFistMastery(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|Special")
	void AddSense(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Stat|Special")
	void AddNegotiation(int32 Value);

	//Getter
	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetCurrentHP() const { return ResourceStat.CurrentHP; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetMaxHP() const { return ResourceStat.MaxHP; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetCurrentInternalEnergy() const { return ResourceStat.CurrentInternalEnergy; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetMaxInternalEnergy() const { return ResourceStat.MaxInternalEnergy; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetAttackPower() const { return CombatStat.AttackPower; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetDefensePower() const { return CombatStat.DefensePower; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetHitRate() const { return CombatStat.HitRate; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetCriticalRate() const { return CombatStat.CriticalRate; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetMoveSpeed() const { return CombatStat.MoveSpeed; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetTravelMoveSpeed() const { return TravelStat.TravelMoveSpeed; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetFateEncounterChance() const { return TravelStat.FateEncounterChance; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetFateGradeBonus() const { return TravelStat.FateGradeBonus; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetLightnessSkill() const { return BaseStat.LightnessSkill; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetInternalEnergy() const { return BaseStat.InternalEnergy; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetVitality() const { return BaseStat.Vitality; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetSwordMastery() const { return WeaponMasteryStat.SwordMastery; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetSpearMastery() const { return WeaponMasteryStat.SpearMastery; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetHiddenWeaponMastery() const { return WeaponMasteryStat.HiddenWeaponMastery; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetFistMastery() const { return WeaponMasteryStat.FistMastery; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetSense() const { return SpecialStat.Sense; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	int32 GetNegotiation() const { return SpecialStat.Negotiation; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetCurrentStamina() const { return ResourceStat.CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stat|Getter")
	float GetMaxStamina() const { return ResourceStat.MaxStamina; }

private:
	void ClampResources();

	void BroadcastStatChanged();
};
