#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "Muksi/Contents/Travel/Public/Data/Stats/CharacterStatDataAsset.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Travel/Public/Components/Player/EquipmentComponent.h"
#include "Muksi/Save/MuksiSaveSubsystem.h"
#include "Muksi/Contents/Travel/Public/Data/Stats/CharacterStatFormulaDataAsset.h"
#include "Curves/CurveFloat.h"

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

	if (!IsValid(StatDataAsset) || !IsValid(StatFormulaDataAsset))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponent: StatDataAsset is null on %s"), *OwnerName);
		return;
	}

	// 초기 투자 능력치 가져오기
	BaseStat = StatDataAsset->BaseStat;
	
	// 최종 능력치 초기화
	ResourceStat = FResourceStat{};
	CombatStat = FCombatStat{};
	TravelStat = FTravelStat{};

	// 스킬 및 패시브
	BaseSkillDataAssets = StatDataAsset->BaseSkillDataAssets;
	BasePassives = StatDataAsset->BasePassives;

	// Curve 기반 능력치 계산
	RecalculateStats();

	// 최초 생성 시 자원 최대치 설정
	ResourceStat.CurrentHP = ResourceStat.MaxHP;
	ResourceStat.CurrentInternalEnergy = ResourceStat.MaxInternalEnergy;

	ClampResources();

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

void UStatComponent::BindSaveGame()
{
	UMuksiSaveSubsystem* SaveSubsystem = UMuksiSaveSubsystem::Get(this);
	SaveSubsystem->OnSaveRequested.AddUObject(this, &UStatComponent::SaveStat);
	SaveSubsystem->OnLoadRequested.AddUObject(this, &UStatComponent::LoadStat);
}

void UStatComponent::UnbindSaveGame()
{
	
}


void UStatComponent::RecalculateStats()
{
	if (!IsValid(StatFormulaDataAsset))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponent: StatFormulaDataAsset skipped because StatDataAsset is null on %s"),
			*GetNameSafe(GetOwner()));
		return;
	}

	const FStatFormulaBaseValues& BaseValues = StatFormulaDataAsset->BaseValues;
	
	// ============================
	// 장비 보정값 가져오기
	// ============================

	FMuksiItemStatModifier EquipmentModifier;

	if (const AActor* Owner = GetOwner())
	{
		if (const UEquipmentComponent* EquipmentComponent = Owner->FindComponentByClass<UEquipmentComponent>())
		{
			EquipmentComponent->GetTotalEquipmentStatModifier(EquipmentModifier);
		}
	}
	
	// ============================
	// Curve 계산 함수
	// ============================

	const auto EvalCurve = [](const UCurveFloat* Curve, int32 Points) -> float
		{
			if (!Curve)
			{
				return 0.f;
			}

			return Curve->GetFloatValue(static_cast<float>(FMath::Max(0, Points)));
		};
	
	// ============================
	// 기본 투자 포인트 + 장비 보정
	// ============================

	const int32 EffectiveLightnessSkill = BaseStat.LightnessSkill + EquipmentModifier.LightnessSkill;

	const int32 EffectiveInternalEnergy = BaseStat.InternalEnergy + EquipmentModifier.InternalEnergy;

	const int32 EffectiveVitality = BaseStat.Vitality + EquipmentModifier.Vitality;

	const int32 EffectivePower = BaseStat.Power + EquipmentModifier.Power;

	const int32 EffectiveSense = BaseStat.Sense + EquipmentModifier.Sense;

	const int32 EffectiveNegotiation = BaseStat.Negotiation + EquipmentModifier.Negotiation;
	
	//최종 능력치 = 
	//기본값 + 
	//(플레이어가 찍은 Stat + 장비로 올라가는 스탯(ex) Vitality +5))를 x로 한 보정용 Curve의 Y값 + 
	//장비추가능력치(ex) 최대 체력 +10)

	// ============================
	// ResourceStat
	// ============================

	ResourceStat.MaxHP = BaseValues.MaxHP + EvalCurve(StatFormulaDataAsset->MaxHPCurve.Get(), EffectiveVitality) + EquipmentModifier.MaxHP;

	ResourceStat.MaxInternalEnergy = BaseValues.MaxInternalEnergy + FMath::FloorToFloat(EvalCurve(StatFormulaDataAsset->MaxInternalEnergyCurve.Get()
		, EffectiveInternalEnergy))
		+ EquipmentModifier.MaxInternalEnergy;
	
	// ============================
	// CombatStat
	// ============================

	CombatStat.AttackPower = BaseValues.AttackPower + EvalCurve(StatFormulaDataAsset->AttackPowerCurve.Get(), EffectivePower) + EquipmentModifier.AttackPower;
	CombatStat.DefensePower = BaseValues.DefensePower + EvalCurve(StatFormulaDataAsset->DefensePowerCurve.Get(), EffectiveVitality) + EquipmentModifier.DefensePower;
	CombatStat.MoveSpeed = BaseValues.MoveSpeed + EvalCurve(StatFormulaDataAsset->MoveSpeedCurve.Get(), EffectiveLightnessSkill) + EquipmentModifier.MoveSpeed;

	// ============================
	// TravelStat
	// ============================

	TravelStat.TravelMoveSpeed = BaseValues.TravelMoveSpeed + EvalCurve(StatFormulaDataAsset->TravelMoveSpeedCurve.Get(), EffectiveLightnessSkill) + EquipmentModifier.TravelMoveSpeed;
	TravelStat.FateEncounterChance = BaseValues.FateEncounterChance + EvalCurve(StatFormulaDataAsset->FateEncounterChanceCurve.Get(), EffectiveSense)+ EquipmentModifier.FateEncounterChance;
	TravelStat.FateGradeBonus = EvalCurve(StatFormulaDataAsset->FateGradeBonusCurve.Get(), EffectiveSense) + EquipmentModifier.FateGradeBonus;
	TravelStat.NegotiationPower = EvalCurve(StatFormulaDataAsset->NegotiationPowerCurve.Get(), EffectiveNegotiation) + EquipmentModifier.NegotiationPower;
	
	// 음수 최대 자원 방지
	ResourceStat.MaxHP = FMath::Max(0.f, ResourceStat.MaxHP);
	ResourceStat.MaxInternalEnergy = FMath::Max(0.f, ResourceStat.MaxInternalEnergy);
	ClampResources();
	
	/*const FResourceStat InitialResourceStat = StatDataAsset->InitialResourceStat;
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

	ClampResources();*/
}

void UStatComponent::RefreshStats()
{
	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::SaveStat(UMuksiSaveGame* SaveGame)
{
	
}

void UStatComponent::LoadStat(const UMuksiSaveGame* SaveGame)
{
	
}

void UStatComponent::ClampResources()
{
	ResourceStat.CurrentHP = FMath::Clamp(ResourceStat.CurrentHP, 0.f, ResourceStat.MaxHP);
	ResourceStat.CurrentInternalEnergy = FMath::Clamp(ResourceStat.CurrentInternalEnergy, 0.f, ResourceStat.MaxInternalEnergy);
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

void UStatComponent::AddPower(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	BaseStat.Power = FMath::Max(0, BaseStat.Power + Value);

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

	BaseStat.Sense += Value;

	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::AddNegotiation(int32 Value)
{
	if (Value == 0)
	{
		return;
	}

	BaseStat.Negotiation += Value;

	RecalculateStats();
	BroadcastStatChanged();
}

void UStatComponent::BroadcastStatChanged()
{
	OnStatChanged.Broadcast();
}