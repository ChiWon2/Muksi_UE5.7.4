#include "Components/StatComponent.h"
#include "Data/CharacterStatDataAsset.h"
#include "GameFramework/Actor.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeStat();
}

void UStatComponent::InitializeStat()
{
	AActor* Owner = GetOwner();
	const FString OwnerName = GetNameSafe(Owner);

	if (!StatDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponent: StatDataAsset is null on %s"), *OwnerName);
		return;
	}

	BaseStat = StatDataAsset->BaseStat;
	WeaponMasteryStat = StatDataAsset->WeaponMasteryStat;
	SpecialStat = StatDataAsset->SpecialStat;
	TravelStat = StatDataAsset->BaseTravelStat;
	ResourceStat = StatDataAsset->InitialResourceStat;
	CombatStat = StatDataAsset->BaseCombatStat;

	RecalculateStats();

	ResourceStat.CurrentHP = ResourceStat.MaxHP;
	ResourceStat.CurrentInternalEnergy = ResourceStat.MaxInternalEnergy;
	ResourceStat.CurrentStamina = ResourceStat.MaxStamina;
	ClampResources();

	UE_LOG(LogTemp, Log, TEXT("StatComponent Initialized: Owner=%s"), *OwnerName);

	BroadcastStatChanged();
}


void UStatComponent::RecalculateStats()
{
	if (!StatDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponent: RecalculateStats skipped because StatDataAsset is null on %s"),
			*GetNameSafe(GetOwner()));
		return;
	}

	const FResourceStat InitialResourceStat = StatDataAsset->InitialResourceStat;
	const FCombatStat BaseCombatStat = StatDataAsset->BaseCombatStat;
	const FTravelStat BaseTravelStat = StatDataAsset->BaseTravelStat;

	ResourceStat.MaxHP = InitialResourceStat.MaxHP + BaseStat.Vitality * 10.f;
	ResourceStat.MaxInternalEnergy = InitialResourceStat.MaxInternalEnergy + BaseStat.InternalEnergy * 8.f;
	ResourceStat.InternalEnergyRegenPerTurn = InitialResourceStat.InternalEnergyRegenPerTurn + BaseStat.InternalEnergy * 0.2f;
	ResourceStat.MaxStamina = InitialResourceStat.MaxStamina + BaseStat.LightnessSkill * 5.f;

	CombatStat.AttackPower = BaseCombatStat.AttackPower;
	CombatStat.DefensePower = BaseCombatStat.DefensePower + BaseStat.Vitality * 1.f;
	CombatStat.HitRate = BaseCombatStat.HitRate;
	CombatStat.CriticalRate = BaseCombatStat.CriticalRate;
	CombatStat.MoveSpeed = BaseCombatStat.MoveSpeed + BaseStat.LightnessSkill * 2.f;

	TravelStat.TravelMoveSpeed = BaseTravelStat.TravelMoveSpeed + BaseStat.LightnessSkill * 2.f;
	TravelStat.FateEncounterChance = BaseTravelStat.FateEncounterChance + SpecialStat.Sense * 0.001f;
	TravelStat.FateGradeBonus = BaseTravelStat.FateGradeBonus + SpecialStat.Sense * 0.001f;

	ClampResources();
}

void UStatComponent::ClampResources()
{
	ResourceStat.CurrentHP = FMath::Clamp(ResourceStat.CurrentHP, 0.f, ResourceStat.MaxHP);
	ResourceStat.CurrentInternalEnergy = FMath::Clamp(ResourceStat.CurrentInternalEnergy, 0.f, ResourceStat.MaxInternalEnergy);
	ResourceStat.CurrentStamina = FMath::Clamp(ResourceStat.CurrentStamina, 0.f, ResourceStat.MaxStamina);
}

void UStatComponent::Heal(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}

	const float OldHP = ResourceStat.CurrentHP;

	ResourceStat.CurrentHP += Amount;
	ClampResources();

	if (!FMath::IsNearlyEqual(OldHP, ResourceStat.CurrentHP))
	{
		BroadcastStatChanged();
	}
}

void UStatComponent::RecoverInternalEnergy(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}

	const float OldValue = ResourceStat.CurrentInternalEnergy;

	ResourceStat.CurrentInternalEnergy += Amount;
	ClampResources();

	if (!FMath::IsNearlyEqual(OldValue, ResourceStat.CurrentInternalEnergy))
	{
		BroadcastStatChanged();
	}
}


bool UStatComponent::UseInternalEnergy(float Amount)
{
	if (Amount <= 0.f)
	{
		return false;
	}

	if (ResourceStat.CurrentInternalEnergy < Amount)
	{
		return false;
	}

	ResourceStat.CurrentInternalEnergy -= Amount;
	ClampResources();

	BroadcastStatChanged();
	return true;
}


void UStatComponent::TakeDamage(float Damage)
{
	if (Damage <= 0.f)
	{
		return;
	}

	const float OldHP = ResourceStat.CurrentHP;

	ResourceStat.CurrentHP -= Damage;
	ClampResources();

	if (!FMath::IsNearlyEqual(OldHP, ResourceStat.CurrentHP))
	{
		BroadcastStatChanged();
	}
}


void UStatComponent::AddLightnessSkill(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	BaseStat.LightnessSkill += Value;
	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::AddInternalEnergy(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	BaseStat.InternalEnergy += Value;
	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::AddVitality(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	BaseStat.Vitality += Value;
	RecalculateStats();
	BroadcastStatChanged();
}


void UStatComponent::AddSwordMastery(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	WeaponMasteryStat.SwordMastery += Value;
	BroadcastStatChanged();
}

void UStatComponent::AddSpearMastery(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	WeaponMasteryStat.SpearMastery += Value;
	BroadcastStatChanged();
}

void UStatComponent::AddHiddenWeaponMastery(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	WeaponMasteryStat.HiddenWeaponMastery += Value;
	BroadcastStatChanged();
}

void UStatComponent::AddFistMastery(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	WeaponMasteryStat.FistMastery += Value;
	BroadcastStatChanged();
}


void UStatComponent::AddSense(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	SpecialStat.Sense += Value;
	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::AddNegotiation(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	SpecialStat.Negotiation += Value;
	BroadcastStatChanged();
}

void UStatComponent::BroadcastStatChanged()
{
	OnStatChanged.Broadcast();
}