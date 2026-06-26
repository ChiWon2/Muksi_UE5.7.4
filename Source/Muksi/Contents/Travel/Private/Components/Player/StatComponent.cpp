#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "Muksi/Contents/Travel/Public/Data/Stats/CharacterStatDataAsset.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Travel/Public/Components/Player/EquipmentComponent.h"

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

	UE_LOG(LogTemp, Warning,
		TEXT("[StatComponent] Initialized Ptr=%p Owner=%s HP=%.0f MaxHP=%.0f ATK=%.0f DEF=%.0f"),
		this,
		*GetNameSafe(GetOwner()),
		ResourceStat.CurrentHP,
		ResourceStat.MaxHP,
		CombatStat.AttackPower,
		CombatStat.DefensePower);
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

	FMuksiItemStatModifier EquipmentModifier;
	if (const AActor* Owner = GetOwner())
	{
		if (const UEquipmentComponent* EquipmentComponent = Owner->FindComponentByClass<UEquipmentComponent>())
		{
			EquipmentComponent->GetTotalEquipmentStatModifier(EquipmentModifier);
		}
	}

	const int32 EffectiveLightnessSkill = BaseStat.LightnessSkill + EquipmentModifier.LightnessSkill;
	const int32 EffectiveInternalEnergy = BaseStat.InternalEnergy + EquipmentModifier.InternalEnergy;
	const int32 EffectiveVitality = BaseStat.Vitality + EquipmentModifier.Vitality;
	const int32 EffectiveSense = SpecialStat.Sense + EquipmentModifier.Sense;

	ResourceStat.MaxHP = InitialResourceStat.MaxHP + EffectiveVitality * 10.f + EquipmentModifier.MaxHP;
	ResourceStat.MaxInternalEnergy = InitialResourceStat.MaxInternalEnergy + EffectiveInternalEnergy * 8.f + EquipmentModifier.MaxInternalEnergy;
	ResourceStat.InternalEnergyRegenPerTurn = InitialResourceStat.InternalEnergyRegenPerTurn + EffectiveInternalEnergy * 0.2f;
	ResourceStat.MaxStamina = InitialResourceStat.MaxStamina + EffectiveLightnessSkill * 5.f + EquipmentModifier.MaxStamina;

	CombatStat.AttackPower = BaseCombatStat.AttackPower + EquipmentModifier.AttackPower;
	CombatStat.DefensePower = BaseCombatStat.DefensePower + EffectiveVitality * 1.f + EquipmentModifier.DefensePower;
	CombatStat.HitRate = BaseCombatStat.HitRate + EquipmentModifier.HitRate;
	CombatStat.CriticalRate = BaseCombatStat.CriticalRate + EquipmentModifier.CriticalRate;
	CombatStat.MoveSpeed = BaseCombatStat.MoveSpeed + EffectiveLightnessSkill * 2.f + EquipmentModifier.MoveSpeed;

	TravelStat.TravelMoveSpeed = BaseTravelStat.TravelMoveSpeed + EffectiveLightnessSkill * 2.f + EquipmentModifier.TravelMoveSpeed;
	TravelStat.FateEncounterChance = BaseTravelStat.FateEncounterChance + EffectiveSense * 0.001f + EquipmentModifier.FateEncounterChance;
	TravelStat.FateGradeBonus = BaseTravelStat.FateGradeBonus + EffectiveSense * 0.001f + EquipmentModifier.FateGradeBonus;

	ClampResources();
}

void UStatComponent::RefreshStats()
{
	RecalculateStats();
	BroadcastStatChanged();
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