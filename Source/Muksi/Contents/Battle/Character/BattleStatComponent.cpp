// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"

// Sets default values for this component's properties
UBattleStatComponent::UBattleStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UBattleStatComponent::InitializeStats(const FBattleStats& InStats)
{
	BaseStats = InStats;
	CurrentStats = InStats;

	CurrentStats.HP = FMath::Max(0.0f, CurrentStats.HP);
	CurrentStats.Attack = FMath::Max(0.0f, CurrentStats.Attack);
	CurrentStats.Defense = FMath::Max(0.0f, CurrentStats.Defense);
	CurrentStats.Speed = FMath::Max(0.0f, CurrentStats.Speed);

	CurrentHP = CurrentStats.HP;
	bIsDead = false;

	OnHPChanged.Broadcast(CurrentHP, CurrentStats.HP);
}

float UBattleStatComponent::ApplyDamage(float Damage)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	if (Damage <= 0.0f)
	{
		return 0.0f;
	}
	
	const float FinalDamage = FMath::Max(1.0f, Damage - CurrentStats.Defense);

	const float PreviousHP = CurrentHP;

	CurrentHP = FMath::Clamp(
		CurrentHP - FinalDamage,
		0.0f,
		CurrentStats.HP
	);

	const float AppliedDamage = PreviousHP - CurrentHP;

	OnHPChanged.Broadcast(CurrentHP, CurrentStats.HP);

	if (CurrentHP <= 0.0f)
	{
		HandleDeath();
	}

	return AppliedDamage;
}

float UBattleStatComponent::Heal(float HealAmount)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	if (HealAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float PreviousHP = CurrentHP;

	CurrentHP = FMath::Clamp(
		CurrentHP + HealAmount,
		0.0f,
		CurrentStats.HP
	);

	const float AppliedHeal = CurrentHP - PreviousHP;

	OnHPChanged.Broadcast(CurrentHP, CurrentStats.HP);

	return AppliedHeal;
}

void UBattleStatComponent::SetCurrentHP(float NewHP)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHP = FMath::Clamp(
		NewHP,
		0.0f,
		CurrentStats.HP
	);

	OnHPChanged.Broadcast(CurrentHP, CurrentStats.HP);

	if (CurrentHP <= 0.0f)
	{
		HandleDeath();
	}
}

void UBattleStatComponent::RestoreFullHP()
{
	if (bIsDead)
	{
		return;
	}

	CurrentHP = CurrentStats.HP;

	OnHPChanged.Broadcast(CurrentHP, CurrentStats.HP);
}

void UBattleStatComponent::SetCurrentSpeed(float NewSpeed)
{
	if (NewSpeed <= 0.0f)NewSpeed = 0.0f;
	CurrentStats.Speed = NewSpeed;
}

void UBattleStatComponent::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHP = 0.0f;

	OnDead.Broadcast();
}




